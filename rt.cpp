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

vec3 phong(const hit_record &hitou, const camera &cam, phongLight light){
    vec3 n,l,r,v;
    

    l = (light.position - hitou.p); // direção da luz
    n = hitou.normal; // normal no ponto que hitou
    v = (cam.origin - hitou.p); // view direction

    vec3 b = unit_vector(l);
    l.normalize();
    v.normalize();
    n.normalize();

    r = 2*dot(l,n)*n - l; // pega o raio refletido pela luz

    float vr = dot(v,r), cosine =  max(dot(n,l), 0.0f); // pega o cosseno entre n e l

    vec3 Ka = hitou.material.color*hitou.material.Ka;
    vec3 Kd = hitou.material.color*hitou.material.Kd;
    vec3 Ks = hitou.material.color*hitou.material.Ks;
    float exponent = 128.0;

    vec3 ambient = light.color*Ka;
    vec3 diffuse = vec3(0,0,0);
    vec3 specular = vec3(0,0,0);
    
    if(cosine > 0.0) {
       diffuse = Kd * light.color * cosine;
       specular = Ks*light.color*pow(max(0.0f, vr),exponent);
    }
    return  hitou.material.alpha*(ambient + diffuse + specular);

}

vec3 color(const ray& r, hitable *world, const camera &cam, const phongLight &light){
    hit_record rec;
    if(world->hit(r,0.0000001,FLT_MAX,rec)){ // se acertar algum objeto da imagem, entra nesse if
        hit_record h;
        if(world->hit(ray(rec.p, light.position-rec.p), 0.0001,FLT_MAX, h)) {
                return vec3(0.0,0.0,0.0);
        }
        return phong(rec,cam,light);
    }
    else{
        return vec3(0.0,0.0,0.0);
    }
}

//
int main(){
    int W = 500; // tamanho horizontal da tela
    int H = 500; // tamanho vertical da tela
    int ns = 5; // precisão do antialiasing
    camera cam(vec3(-3.0,3.0,-3.0), vec3(0.0,0.0,0.0), vec3(0.0,1.0,0.0), 90, float(W)/float(H));


    fstream cena;
    cena.open("cenaze.txt");
    string action;
    cena >> action;
   
    while(cena >> action){
        if(action == "res"){
            cin >> H >> W;
        }else if(action == "camera"){
            float px,py,pz,tx,ty,tz,ux,uy,uz,fov,f;
            cin >> px >> py >> pz >> tx >> ty >> tz >> ux >> uy >> uz >> fov >> f;
            cam = camera(vec3(px,py,pz), vec3(tx,ty,tz), vec3(ux,uy,uz), fov, f);
        }else if(action == "material"){

        }else if(action == "sphere")

    }

    
    //ler cenaze aqui, salvar materiais em um map, esferas em um vector, depois inicializar world usando isso
    
    int QUANTIDADE = 12;//tamanho do vector de esferas 
    hitable *list[QUANTIDADE]; // array de objetos na imagem (inicializar com as esferas)

    

    vec3 LIGHTPOSITION = vec3(1.0,2.5,0.0);
    hitable *world = new hitable_list(list,QUANTIDADE); // objeto que tem todas as imagens


    phongLight light(BRANCO, LIGHTPOSITION); // 1 parametro é a cor, segundo é a posição
    // camera: 1 parametro é a posição da camera, segundo é o alvo, terceiro é o vetor up, quarto é o fov (vertical), quinto é o aspect/ratio
   
    ofstream out("teste.ppm");//arquivo resultado
    out << "P3" << '\n' << W << '\n' << H << '\n' << "255" << '\n'; 
    for(int j = H-1; j >= 0; j--){ // começa a preencher a imagem de cima para baixo
        for(int i = 0; i < W; i++){ // e da esquerda para a direita
            vec3 col(0,0,0); 
            for(int s = 0;s < ns;s++) { // for do anti-aliasing: recomendo ler sobre para entender
                float u = float(i + drand48()/*random_digit()*/) / float(W);
                float v = float(j + drand48()/*random_digit()*/) / float(H);
                ray r = cam.get_ray(u,v);
                col += color(r,world,cam,light);
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