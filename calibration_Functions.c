#include "calibration_Functions.h"

struct Point{
    float x;
    float y;
};
int n;
struct Point points[n];


float x_to_y(Point points[], float x){
    if(n < 2)
        return 0;
    struct Point point1 = points[0];
    struct Point point2 = points[1];
    
    for(int i=0; i<n-1; i++){
        point1 = points[i];
        point2 = points[i+1];
        if(x <= point2.x)
            break;
    }
    return((((x-point1.x)*(point2.y-point1.y))/(point2.x-point1.x))+point1.y);
}

float y_to_x(Point points[], float y){
    if(n < 2)
        return 0;
    struct Point point1 = points[0];
    struct Point point2 = points[1];
    
    for(int i=0; i<n-1; i++){
        point1 = points[i];
        point2 = points[i+1];
        if( <= point2.y)
            break;
    }
    return((((y-point1.y)*(point2.x-point1.x))/(point2.y-point1.y))+point1.x);
}