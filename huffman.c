/*
HUFF2412 File Format Specification (Encoder v1)

Overview
--------
This program writes a Huffman-compressed output file. The output is divided into:

  [HEADER (byte-aligned parts + bit-packed tree)] [PAYLOAD (bit-packed codes)]

"Payload" = the compressed bitstream for the original file contents, written
by replacing each input byte with its Huffman code bits.

All multi-byte integer fields are written using the platform's native in-memory
representation (typically little-endian). For cross-platform portability,
define and use an explicit byte order.

Header Layout (in order)
------------------------
1) Magic (8 bytes)
   ASCII bytes: 'H' 'U' 'F' 'F' '2' '4' '1' '2'
   Used to identify files produced by this compressor.

2) Original Size (uint64_t, 8 bytes)
   The exact number of bytes in the original uncompressed input file.
   Decoder stops after producing this many bytes (ignores any padding bits).

3) Stored Filename Length (uint16_t, 2 bytes)
   Length in bytes of the stored filename string.

4) Stored Filename (nameLen bytes)
   Raw bytes of the input file base name (includes extension), not NUL-terminated.
   Example: "photo.png" or "archive.tar.gz".

5) Tree Bit Length (uint32_t, 4 bytes)
   Number of bits used to encode the serialized Huffman tree that immediately follows.

6) Serialized Huffman Tree (treeBitsLen bits, bit-packed)
   Preorder serialization:
     - Leaf node: write bit 1, then write 8 bits of the leaf byte value.
       (Total = 9 bits per leaf.)
     - Internal node: write bit 0, then serialize left subtree, then right subtree.
       (Total = 1 bit + left + right.)

7) Tree Padding (0..7 bits)
   After the tree bits are written, the encoder pads with 0 bits up to the next
   byte boundary so that the payload starts on a byte boundary.

Payload Layout
--------------
8) Huffman Payload Bits (variable length, bit-packed)
   For each byte read from the input file, write the corresponding Huffman code
   bits from the code table.

9) Final Padding (0..7 bits)
   After all symbols are written, the encoder pads the last output byte with 0 bits
   (if needed) and flushes it.

Notes / Constraints
-------------------
- Empty input files are currently rejected by the encoder.
- Codes are limited to 64 bits in this implementation (ERR_RECURSION_DEPTH if exceeded).
- The stored treeBitsLen allows a decoder to know exactly where the tree ends and
  where the payload begins.
*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define BUFFER_SIZE 8192
#define FREQ_SIZE 256

typedef enum {
    SUCCESS,
    ERR_MEMORY,
    ERR_FILE,
    ERR_READ,
    ERR_HEAP_CAPACITY_FULL,
    ERR_RECURSION_DEPTH
} Status;

/*========================= READ FILE ===========================*/

Status readFile(const char* filename, uint64_t **freq, uint64_t *totalBytes) {
    FILE *f = fopen(filename, "rb");
    
    if(!f) {
        return ERR_FILE;
    }
    
    *freq = (uint64_t*) calloc(FREQ_SIZE, sizeof(uint64_t));
    if(!*freq) {
        fclose(f);
        return ERR_MEMORY;
    }
    
    uint8_t *buffer = (uint8_t*) malloc(BUFFER_SIZE);
    if(!buffer) {
        free(*freq);
        *freq = NULL;
        fclose(f);
        return ERR_MEMORY;
    }
    
    size_t bytesRead = 0;
    *totalBytes = 0;
    
    while((bytesRead = fread(buffer, 1, BUFFER_SIZE, f)) > 0) {
        *totalBytes += bytesRead;
        
        for(size_t i=0; i<bytesRead; i++) {
            (*freq)[buffer[i]]++;
        }
    }
    
    if(ferror(f)) {
        free(*freq);
        *freq = NULL;
        free(buffer);
        fclose(f);
        return ERR_READ;
    }
    
    free(buffer);
    fclose(f);
    return SUCCESS;
}

/*============================== NODE ===============================*/

typedef struct Node {
    uint8_t byte;
    uint64_t freq;
    struct Node *left;
    struct Node *right;
} Node;

Status createNode(Node **newNode, uint8_t byte, uint64_t freq, Node *left, Node *right) {
    *newNode = (Node*) malloc(sizeof(Node));
    if(!*newNode) {
        return ERR_MEMORY;
    }

    (*newNode)->byte = byte;
    (*newNode)->freq = freq;
    (*newNode)->left = left;
    (*newNode)->right = right;

    return SUCCESS;
}

void freeTree(Node **root) {
    if(!root || !*root) return;
    freeTree(&(*root)->left);
    freeTree(&(*root)->right);
    free(*root);
    *root = NULL;
}

/*============================== HEAP ===============================*/

typedef struct {
    Node **arr;
    size_t size;
    size_t capacity;
} MinHeap;

void swap(Node **a, Node **b) {
    Node *tmp = *a;
    *a = *b;
    *b = tmp;
}

void heapifyDown(MinHeap *h, size_t i) {
    size_t smallest = i;
    size_t left = 2*i + 1;
    size_t right = 2*i + 2;

    if(left < h->size && h->arr[left]->freq < h->arr[smallest]->freq) {
        smallest = left;
    }

    if(right < h->size && h->arr[right]->freq < h->arr[smallest]->freq) {
        smallest = right;
    }

    if(smallest != i) {
        swap(&h->arr[i], &h->arr[smallest]);
        heapifyDown(h, smallest);
    }
}

void heapifyUp(MinHeap *h, size_t i) {
    if(i == 0) {
        return;
    }

    size_t parent = (i - 1)/2;

    if(h->arr[i]->freq < h->arr[parent]->freq) {
        swap(&h->arr[i], &h->arr[parent]);
        heapifyUp(h, parent);
    }
}

Status heapInsert(MinHeap *h, Node *node) {
    if(h->size >= h->capacity) return ERR_HEAP_CAPACITY_FULL;
    
    h->arr[h->size] = node;
    heapifyUp(h, h->size);
    h->size++;

    return SUCCESS;
}

Node* extractMin(MinHeap *h) {
    if(h->size == 0) return NULL;

    Node *min = h->arr[0];
    h->arr[0] = h->arr[h->size-1];
    h->size--;

    heapifyDown(h, 0);
    return min;
}

void destroyHeapStruct(MinHeap **h) {
    if(!h || !*h) return;
    free((*h)->arr);
    free(*h);
    *h = NULL;
}

void destroyHeapWithNodes(MinHeap **h) {
    if(!h || !*h) return;

    if((*h)->arr) {
        for(size_t i=0; i<(*h)->size; i++) {
            freeTree(&(*h)->arr[i]);
        }
    }

    free((*h)->arr);
    free(*h);
    *h = NULL;
}

/*============================ BUILD HEAP ==============================*/

Status buildHeap(MinHeap **h, const uint64_t *freq) {
    *h = NULL;
    Status status = SUCCESS;

    *h = (MinHeap*) malloc(sizeof(MinHeap));
    if(!*h) {
        return ERR_MEMORY;
    }

    (*h)->capacity = FREQ_SIZE;
    (*h)->size = 0;
    (*h)->arr = (Node**) malloc((*h)->capacity * sizeof(Node*));
    if(!(*h)->arr) {
        free(*h);
        return ERR_MEMORY;
    }

    for(size_t i=0; i<FREQ_SIZE; i++) {
        if(freq[i] > 0) {
            Node *newNode = NULL;

            status = createNode(&newNode, i, freq[i], NULL, NULL);
            if(status != SUCCESS) goto cleanup;

            status = heapInsert(*h, newNode);
            if(status != SUCCESS) {
                free(newNode);
                goto cleanup;
            }
        }
    }

    return SUCCESS;

    cleanup:
        destroyHeapWithNodes(h);
        return status;
}

/*======================= BUILD HUFFMAN TREE ==========================*/

Status buildHuffmanTree(MinHeap *h, Node **root) {
    Status status = SUCCESS;
    *root = NULL;

    if(!h || h->size == 0) {
        return SUCCESS;
    }

    while(h->size > 1) {
        Node *a = extractMin(h);
        Node *b = extractMin(h);

        if(!a || !b) {
            freeTree(&a);
            freeTree(&b);
            return ERR_FILE;
        }
        
        Node *merged = NULL;

        status = createNode(&merged, 0, a->freq + b->freq, a, b);
        if(status != SUCCESS) {
            freeTree(&a);
            freeTree(&b);
            return status;
        }

        status = heapInsert(h, merged);
        if(status != SUCCESS) {
            freeTree(&merged);
            return status;
        }
    }

    *root = extractMin(h);
    h->size = 0;
    return SUCCESS;
}

uint64_t treeBitLength(const Node *root) {
    if(!root) return 0;

    if(!root->left && !root->right) {
        return 1+8;
    }

    return 1 + treeBitLength(root->left) + treeBitLength(root->right);
}

/*======================= GENERATE HUFFMAN CODES =========================*/

typedef struct {
    uint64_t bits;
    uint8_t length;
} Code;

Status initTable(Code **table) {
    *table = (Code*) calloc(256, sizeof(Code));
    if(!*table) return ERR_MEMORY;
    return SUCCESS;
}

Status buildCodes(const Node *root, Code *table, uint64_t code, uint8_t depth) {
    if(!root) return SUCCESS;

    if(depth >= 64) {
        return ERR_RECURSION_DEPTH;
    }

    // LEAF
    if(!root->left && !root->right) {
        // single symbol case
        if(depth == 0) {
            table[root->byte].bits = 0;
            table[root->byte].length = 1;
        }

        else {
            table[root->byte].bits = code;
            table[root->byte].length = depth;
        }

        return SUCCESS;
    }

    Status s;

    // go left -> 0
    s = buildCodes(root->left, table, (code << 1) | 0, depth+1);
    if(s!= SUCCESS) return s;
    
    // go right -> 1
    s = buildCodes(root->right, table, (code << 1) | 1, depth+1);
    if(s!= SUCCESS) return s;

    return SUCCESS;
}

/*===================== BIT WRITER AND UTILS =======================*/

typedef struct {
    FILE *f;
    uint8_t buffer;
    uint8_t bitCount;
} BitWriter;

Status initBitWriter(BitWriter *bw, const char *filename) {
    bw->f = fopen(filename, "wb");
    if(!bw->f) {
        return ERR_FILE;
    }

    bw->buffer = bw->bitCount = 0;
    return SUCCESS;
}

Status writeBits(BitWriter *bw, uint64_t bits, uint8_t length) {
    for(int i=length-1; i>=0; i--) {
        uint8_t bit = (uint8_t) ((bits >> i) & 1);

        bw->buffer = (uint8_t) ((bw->buffer << 1) | bit);
        bw->bitCount++;

        if(bw->bitCount == 8) {
            if(fwrite(&bw->buffer, 1, 1, bw->f) != 1) return ERR_FILE;

            bw->buffer = 0;
            bw->bitCount = 0;
        }
    }

    return SUCCESS;
}

Status writePaddedBits(BitWriter *bw) {
    // pads leftover bits with 0s and writes them to file
    if(bw->bitCount == 0) return SUCCESS;
    bw->buffer = (uint8_t) (bw->buffer << (8-bw->bitCount));

    if(fwrite(&bw->buffer, 1, 1, bw->f) != 1) return ERR_FILE;

    bw->buffer = bw->bitCount = 0;
    return SUCCESS;
}

Status closeBitWriter(BitWriter *bw) {
    if(!bw || !bw->f) return SUCCESS;
    if(fclose(bw->f) != 0) return ERR_FILE;
    bw->f = NULL;
    return SUCCESS;
}

const char* baseName(const char *path) {
    if(!path) return "";

    const char *last = path;
    for(const char *p = path; *p; p++) {
        if(*p == '/' || *p == '\\') last = p + 1;
    }
    return last;
}

/*========================= HEADER ============================*/

Status writeTree(const Node *root, BitWriter *bw) {
    if(!root) return ERR_FILE;

    Status s;

    // leaf
    if(!root->left && !root->right) {
        s = writeBits(bw, 1, 1);
        if(s != SUCCESS) return s;
        return writeBits(bw, root->byte, 8);
    }

    // internal
    s = writeBits(bw, 0, 1);
    if(s != SUCCESS) return s;

    s = writeTree(root->left, bw);
    if(s != SUCCESS) return s;
    
    s = writeTree(root->right, bw);
    if(s != SUCCESS) return s;

    return SUCCESS;
}

Status writeHeader(BitWriter *bw, uint64_t originalSize, const Node *root, const char* inputPath) {
    const uint8_t magic[] = {'H', 'U', 'F', 'F', '2', '4', '1', '2'};

    if(fwrite(magic, 1, 8, bw->f) != 8) return ERR_FILE;

    if(fwrite(&originalSize, sizeof(originalSize), 1, bw->f) != 1) return ERR_FILE;

    const char *name = baseName(inputPath);
    size_t n = strlen(name);
    if(n > UINT16_MAX) return ERR_FILE;

    uint16_t nameLen = (uint16_t) n;

    if(fwrite(&nameLen, sizeof(nameLen), 1, bw->f) != 1) return ERR_FILE;
    if(nameLen > 0) {
        if(fwrite(name, 1, nameLen, bw->f) != nameLen) return ERR_FILE;
    }

    uint64_t treeBits64 = treeBitLength(root);
    if(treeBits64 > UINT32_MAX) return ERR_FILE;

    uint32_t treeBitsLen = (uint32_t) treeBits64;
    if(fwrite(&treeBitsLen, sizeof(treeBitsLen), 1, bw->f) != 1) return ERR_FILE;

    Status s = writeTree(root, bw);
    if(s != SUCCESS) return s;
    
    s = writePaddedBits(bw);
    if(s != SUCCESS) return s;
    
    return SUCCESS;
}

/*========================= ENCODER ===========================*/

Status encodeFile(const char *inName, const char *outName, const Code *table, uint64_t originalSize, const Node *root) {
    Status s = SUCCESS;
    FILE *in = NULL;
    uint8_t *buffer = NULL;
    BitWriter bw = {0};
    
    in = fopen(inName, "rb");
    if(!in) return ERR_FILE;
    
    s = initBitWriter(&bw, outName);
    if(s != SUCCESS) goto cleanup;
    
    buffer = (uint8_t*) malloc(BUFFER_SIZE);
    if(!buffer) {
        s = ERR_MEMORY;
        goto cleanup;
    }

    s = writeHeader(&bw, originalSize, root, inName);
    if(s != SUCCESS) goto cleanup;

    size_t bytesRead = 0;

    while((bytesRead = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
        for(size_t i=0; i<bytesRead; i++) {
            uint8_t byte = buffer[i];

            if(table[byte].length == 0) {
                s = ERR_FILE;
                goto cleanup;
            }

            s = writeBits(&bw, table[byte].bits, table[byte].length);
            if(s != SUCCESS) goto cleanup;
        }
    }

    if(ferror(in)) {
        s = ERR_READ;
        goto cleanup;
    }

    s = writePaddedBits(&bw);
    if(s != SUCCESS) goto cleanup;

    cleanup:
        if(in) fclose(in);

        Status cs = closeBitWriter(&bw);
        if(s == SUCCESS && cs != SUCCESS) s = cs;

        free(buffer);
        return s;
}

/*======================== UTILITIES ========================*/

void cleanupAll(uint64_t *freq, MinHeap *h, Node *root, Code *table, char *outName) {
    free(freq);

    if(root) {
        freeTree(&root);        // frees all nodes
        destroyHeapStruct(&h);  // heap contains stale pointers, but only struct is freed
    } 
    
    else {
        destroyHeapWithNodes(&h); // frees nodes when tree not formed
    }

    free(table);
    free(outName);
}

void printCodes(const Code *table) {
    for (int byte = 0; byte < 256; byte++) {
        if (table[byte].length == 0) continue;

        printf("%c: ", byte);

        uint64_t bits = table[byte].bits;
        uint8_t length = table[byte].length;

        for (uint8_t i = length; i > 0; i--) {
            putchar(((bits >> (i - 1)) & 1) ? '1' : '0');
        }

        putchar('\n');
    }
}

void printStatus(const char *testName, Status s) {
    fprintf(stderr, "%s: ", testName);
    switch (s) {
        case SUCCESS:
        fprintf(stderr, "[SUCCESS]\n");
        break;
        
        case ERR_FILE:
        fprintf(stderr, "[ERR_FILE]\n");
        break;
        
        case ERR_MEMORY:
        fprintf(stderr, "[ERR_MEMORY]\n");
        break;
        
        case ERR_READ:
        fprintf(stderr, "[ERR_READ]\n");
        break;

        case ERR_RECURSION_DEPTH:
        fprintf(stderr, "[ERR_RECURSION_DEPTH]\n");
        break;
        
        case ERR_HEAP_CAPACITY_FULL:
        fprintf(stderr, "[ERR_HEAP_CAPACITY_FULL]\n");
        break;
        }
}

Status makeOutputName(char **outName, const char *inputPath) {
    *outName = NULL;

    const char *base = baseName(inputPath);
    const char *ext = ".huff";

    size_t baseLen = strlen(base);
    size_t extLen = strlen(ext);

    *outName = (char*) malloc(baseLen + extLen + 1);
    if(!*outName) return ERR_MEMORY;

    memcpy(*outName, base, baseLen);
    memcpy(*outName + baseLen, ext, extLen);
    (*outName)[baseLen + extLen] = '\0';

    return SUCCESS;
}

/*====================CMDLINE ARGS PARSING ======================*/

typedef enum {
    NONE,
    COMPRESS,
    DECOMPRESS
} Mode;

Status parseArgs(int argc, char **argv, Mode *mode, const char **inputPath) {
    *mode = NONE;
    *inputPath = NULL;

    if(argc != 3) return ERR_FILE;

    if(strcmp(argv[1], "-c") == 0) {
        *mode = COMPRESS;
        *inputPath = argv[2];
        return SUCCESS;
    }

    else if(strcmp(argv[1], "-x") == 0) {
        *mode = DECOMPRESS;
        *inputPath = argv[2];
        return SUCCESS;
    }

    return ERR_FILE;
}

/*============================ MAIN ============================*/

int main(int argc, char **argv) {
    Mode mode;
    const char *inputPath = NULL;
    Status s;

    s = parseArgs(argc, argv, &mode, &inputPath);
    if(s != SUCCESS || mode == NONE) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s -c <input_file>    (compress)\n", argv[0]);
        fprintf(stderr, "  %s -x <input_file>    (decompress)\n", argv[0]);
        return 1;
    }

    if(mode == COMPRESS) {
        uint64_t *freq = NULL;
        MinHeap *heap = NULL;
        Node *root = NULL;
        uint64_t totalBytes = 0;
        Code *table = NULL;
        char *outName = NULL;

        s = readFile(inputPath, &freq, &totalBytes);
        if(s != SUCCESS) goto cleanup;

        if(totalBytes == 0) {
            s = ERR_FILE;
            goto cleanup;
        }
        
        s = buildHeap(&heap, freq);
        if(s != SUCCESS) goto cleanup;
        
        s = buildHuffmanTree(heap, &root);
        if(s != SUCCESS) goto cleanup;

        s = initTable(&table);
        if(s != SUCCESS) goto cleanup;
        
        s = buildCodes(root, table, 0, 0);
        if(s != SUCCESS) goto cleanup;

        s = makeOutputName(&outName, inputPath);
        if(s != SUCCESS) goto cleanup;
        
        s = encodeFile(inputPath, outName, table, totalBytes, root);
        if(s != SUCCESS) goto cleanup;

        printf("Original File Size: %" PRIu64 " Bytes\n", totalBytes);
        printf("Wrote Compressed File: %s\n", outName);

        cleanup:
            if(s != SUCCESS) {
                printStatus("ERROR", s);
            }

            cleanupAll(freq, heap, root, table, outName);
            return (s == SUCCESS) ? 0 : 1;
    }

    else {
        // decoder
        return 0;
    }
}