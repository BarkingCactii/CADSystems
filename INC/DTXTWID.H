#ifndef _DTXTWID_H
#define _DTXTWID_H

int line_breaks_text(ads_point, ads_point, ads_point, ads_point);
struct resbuf *get_text_poly();
int get_text_bounds(ads_name, ads_point, ads_real, ads_real *);

#endif
