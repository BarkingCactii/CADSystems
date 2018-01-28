#ifndef _GEOM_H
#define _GEOM_H

#define _90_DEG      90.0
#define _180_DEG     180.0
#define _270_DEG     270.0
#define _360_DEG     360.0

double radtodeg(double);
double degtorad(double);
double hypot(double, double);
double distance(ads_point, ads_point);
double delta_x(ads_point, ads_point);
double delta_y(ads_point, ads_point);
double angle (ads_point, ads_point);
int parallel(ads_point, ads_point, ads_point, ads_point);
double adjust_angle(double);
double min_rel_angle(double, double);

#endif
