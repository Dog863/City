#ifndef SVG_H
#define SVG_H

void svg_init(char* file);
void svg_rect(double x, double y, double w, double h, char* fill);
void svg_text(double x, double y, char* txt);
void svg_line(double x1, double y1, double x2, double y2, char* cor);
void svg_circle(double x, double y, double r, char* cor);
void svg_close();

#endif