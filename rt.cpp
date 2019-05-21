//#include<bits/stdc++.h> //inclui tudo mas compila mais lento
#include<iostream>
#include<fstream>
#include<cmath>
#include<algorithm>
#include "sphere.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.h"
#include "material.h"
using namespace std;

vec3 PRETO = vec3(0.0,0.0,0.0), BRANCO = vec3(1.0,1.0,1.0);

vec3 phong(const hit_record &hitou, const camera &cam, const phongLight *lights, const hitable *world){
    vec3 n,l,r,v;
    

    vec3 Ka = hitou.material.color*hitou.material.Ka;
    vec3 Kd = hitou.material.color*hitou.material.Kd;
    vec3 Ks = hitou.material.color*hitou.material.Ks;

    float exponent = 128.0; // coefficient é o Ia

    vec3 coefficient;
    vec3 diffuse = vec3(0,0,0);
    vec3 specular = vec3(0,0,0);

    for(int i = 0;i<2;i++) {
        hit_record h;
        if(world->hit(ray(hitou.p,lights[i].position - hitou.p), 0.001, FLT_MAX, h)) {
            diffuse = diffuse + vec3(0,0,0);
            specular = specular + vec3(0,0,0);
            coefficient = coefficient + lights[i].color;
        } else {
            coefficient = coefficient + lights[i].color;
            l = unit_vector(lights[i].position - hitou.p); // direção da luz
            n = unit_vector(hitou.normal); // normal no ponto que hitou
            v = unit_vector(cam.origin - hitou.p); // view direction

            l.normalize();
            v.normalize();
            n.normalize();

            r = 2*dot(l,n)*n - l; // pega o raio refletido pela luz
            float vr = dot(v,r), cosine =  max(dot(n,l), 0.0f); // pega o cosseno entre n e l

            if(cosine > 0.0) {
                diffuse = diffuse + Kd * lights[i].color * cosine;
                specular = specular + Ks*lights[i].color*pow(max(0.0f, vr),exponent);
           
            }
        }
    }
    
    coefficient = coefficient/2;
    vec3 ambient = coefficient*Ka;
    
    
    return  hitou.material.alpha*(ambient + diffuse + specular);

}

vec3 color(const ray& r, const hitable *world, const camera &cam, const phongLight *lights){
    hit_record rec;
    if(world->hit(r,0.00000001,FLT_MAX,rec)){ // se acertar algum objeto da imagem, entra nesse if
        //hit_record h;
        //if(world->hit(ray(rec.p, lights[0].position-rec.p), 0.001,FLT_MAX, h)) {
        //    return vec3(0.0,0.0,0.0);
        //}
        return phong(rec,cam,lights,world);
    }
    else{
        return vec3(0.0,0.0,0.0);
    }
}

//
int main(){
    const int W = 500; // tamanho horizontal da tela
    const int H = 500; // tamanho vertical da tela
    int ns = 15; // precisão do antialiasing
    ofstream out("teste.ppm");//arquivo resultado
    out << "P3" << '\n' << W << '\n' << H << '\n' << "255" << '\n'; 
    hitable *list[3]; // array de objetos na imagem
    list[0] = new sphere(vec3(0.0,1.0,-1.0),0.2, phongMaterial(vec3(1.0,0.0,0.0), 0.2, 0.5, 0.6, 1.0)); // esfera do centro
    list[1] = new sphere(vec3(0.0,-1000.5,-1.0),1000.0, phongMaterial(vec3(0.0,1.0,0.0), 0.2, 0.5, 0.6, 0.8)); // esfera do "chão"
    //list[2] = new sphere(vec3(0.0,2.0,-1.0), 0.01, phongMaterial(vec3(1.0,1.0,1.0),1.0,1.0,1.0,1.0));
    list[2] = new sphere(vec3(2.0,1.0,-1.0),0.7, phongMaterial(vec3(1.0,0.0,1.0), 0.2, 0.5, 0.6, 1.0)); // esfera do centro
    hitable *world = new hitable_list(list,3); // objeto que tem todas as imagens
    phongLight lights[2];
    lights[0] = phongLight(vec3(1.0,1.0,1.0), vec3(2.0,3.0,-1.0)); // 1 parametro é a cor, segundo é a posição
    //lights[1] = phongLight(vec3(1.0,1.0,1.0), vec3(0.0,3.0,-1.0)); // 1 parametro é a cor, segundo é a posição
    lights[1] = phongLight(vec3(1.0,1.0,1.0), vec3(-2.0,3.0,-1.0)); // 1 parametro é a cor, segundo é a posição
    
    camera cam(vec3(0.0,3.0,4.0), vec3(0.0,0.0,0.0), vec3(0.0,1.0,0.0), 90, float(W)/float(H));
    // camera: 1 parametro é a posição da camera, segundo é o alvo, terceiro é o vetor up, quarto é o fov (vertical), quinto é o aspect/ratio
    for(int j = H-1; j >= 0; j--){ // começa a preencher a imagem de cima para baixo
        for(int i = 0; i < W; i++){ // e da esquerda para a direita
            vec3 col(0,0,0); 
            for(int s = 0;s < ns;s++) { // for do anti-aliasing: recomendo ler sobre para entender
                float u = float(i + drand48()/*random_digit()*/) / float(W);
                float v = float(j + drand48()/*random_digit()*/) / float(H);
                ray r = cam.get_ray(u,v);
                col += color(r,world,cam,lights);
            }

            col /= float(ns);
            col = vec3(col.x,col.y,col.z); // serve pra ajustar a gamma de cores para visualizadores de imagem
            int ir = int(255.99*col.x);  // vermelho do pixel
            int ig = int(255.99*col.y); // verde do pixel
            int ib = int(255.99*col.z); // azul do pixel
            out << ir << " " << ig << " " << ib << "\n";
        }
    }

}        
