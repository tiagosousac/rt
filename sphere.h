#ifndef SPHEREH
#define SPHEREH
#include "hitable.h"
#include "material.h"
class sphere: public hitable {
    public:
    sphere(){}
    sphere(vec3 cen, float r, material *m/*, vec3 col*/) : center(cen), radius(r), mat_ptr(m)/*, color(col)*/{};
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    vec3 center/*,color*/;//Cor foi declarada, mas não está sendo utilizada pela função Cor no main...
    float radius;
    material *mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const{
    vec3 oc = r.origin() - center; //ponto do centro da esfera
    float a = dot(r.direction(), r.direction());
    float b = dot(oc,r.direction()); // 
    float c = dot(oc,oc) - radius*radius;
    float discriminant = b*b - a*c;//"DELTA" - - > talvez tenha um 4 aqui
    if (discriminant > 0) {
        float temp = (-b - sqrt (discriminant)) /a;
        if(temp<t_max && temp>t_min){
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            return true;
        }
        temp = (-b + sqrt(discriminant))/a;
        if(temp < t_max && temp > t_min){
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            return true;
        }
    }
    return false;
}
#endif