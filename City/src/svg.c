#include <stdio.h>
#include "svg.h"

static FILE *f = NULL;

void svg_init(char *file) {
    if (f != NULL) {
        return;
    }
    
    f = fopen(file, "w");
    if (f) {
        fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n");
    }
}

void svg_rect(double x, double y, double w, double h, char *fill) {
    if (f) {
        fprintf(f, "<rect x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" "
                   "style=\"fill:%s;stroke:black;stroke-width:1\"/>\n", 
                   x, y, w, h, fill);
    }
}

void svg_text(double x, double y, char *txt) {
    if (f) {
        fprintf(f, "<text x=\"%lf\" y=\"%lf\" font-size=\"12\" font-family=\"Arial\">%s</text>\n", 
                x, y, txt);
    }
}

void svg_line(double x1, double y1, double x2, double y2, char *cor) {
    if (f) {
        fprintf(f, "<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" "
                   "stroke=\"%s\" stroke-width=\"2\"/>\n", 
                   x1, y1, x2, y2, cor);
    }
}

void svg_circle(double x, double y, double r, char *cor) {
    if (f) {
        fprintf(f, "<circle cx=\"%lf\" cy=\"%lf\" r=\"%lf\" fill=\"%s\" "
                   "stroke=\"black\" stroke-width=\"1\"/>\n", 
                   x, y, r, cor);
    }
}

void svg_close(void) {
    if (f) {
        fprintf(f, "</svg>\n");
        fclose(f);
        f = NULL;
    }
}