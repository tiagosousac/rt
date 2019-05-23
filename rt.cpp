//#include<bits/stdc++.h> //inclui tudo mas compila mais lento
#include<iostream>
#include<fstream>
#include<cmath>
#include<algorithm>
#include<vector>
#include<map>
#include "sphere.h"
#include "plane.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.h"
#include "material.h"
using namespace std;

vec3 PRETO = vec3(0.0,0.0,0.0), BRANCO = vec3(1.0,1.0,1.0);

vec3 phong(const hit_record &hitou, const camera &cam, const hitable_list *world){

    vec3 n,l,r,v;
    

    vec3 Ka = hitou.material.color*hitou.material.Ka;
    vec3 Kd = hitou.material.color*hitou.material.Kd;
    vec3 Ks = hitou.material.color*hitou.material.Ks;

    float alpha = hitou.material.alpha*128; // isso aqui é o alpha do material, ele deve ir de 0.0 a 1.0 preferencialmente

    vec3 ambient = vec3(0.0,0.0,0.0);
    vec3 diffuse = vec3(0.0,0.0,0.0);
    vec3 specular = vec3(0.0,0.0,0.0);

    for(int i = 0;i<world->numLights;i++) {
        hit_record h;
        if(world->hit(ray(hitou.p,world->lights[i].position - hitou.p), 0.001, FLT_MAX, h)) { // entra nesse if se houver interseção entre o ponto e a luz i.
            diffuse = diffuse + vec3(0,0,0);
            specular = specular + vec3(0,0,0);
        } else {
            l = unit_vector(world->lights[i].position - hitou.p); // direção da luz
            n = unit_vector(hitou.normal); // normal no ponto que hitou
            v = unit_vector(cam.origin - hitou.p); // view direction

            r = 2*dot(l,n)*n - l; // pega o raio refletido pela luz
            float vr = dot(v,r), cosine =  max(dot(n,l), 0.0f); // pega o cosseno entre n e l

            if(cosine > 0.0) {
                diffuse = diffuse + Kd * world->lights[i].color * cosine;
                specular = specular + Ks*world->lights[i].color*pow(max(0.0f, vr),alpha);
           
            }
        }
	ambient = ambient + world->lights[i].color;
    }

    plane pla = world->planelight;
    int Y = (int)(pla.p2 - pla.p0).size();
    int X = (int)(pla.p1 - pla.p0).size();
    vec3 vy = pla.p2 - pla.p0;
    vec3 vx = pla.p1 - pla.p0;
    int soft=10;
    for(int i=0;i<soft;i++){
                float uu = drand48()*Y;
                float vv = drand48()*X;
                vec3 ligpos = pla.p0 + uu*vy + vv*vx;//ponto no plano
                hit_record h2;
                if(world->hit(ray(hitou.p,ligpos - hitou.p), 0.001, FLT_MAX, h2)){
                    diffuse = diffuse + vec3(0,0,0);
                    specular = specular + vec3(0,0,0);
                }else{
                    l = unit_vector(ligpos - hitou.p); // direção da luz
                    n = unit_vector(hitou.normal); // normal no ponto que hitou
                    v = unit_vector(cam.origin - hitou.p); // view direction

                    r = 2*dot(l,n)*n - l; // pega o raio refletido pela luz
                    float vr = dot(v,r), cosine =  max(dot(n,l), 0.0f); // pega o cosseno entre n e l

                    if(cosine > 0.0) {
                        diffuse = diffuse + Kd * world->planelight.material.color * cosine;
                        specular = specular + Ks*world->planelight.material.color*pow(max(0.0f, vr),alpha);
           
                    }
                }
                }
            
    
    
	ambient = ambient + world->planelight.material.color;
    ambient = ambient/world->numLights;
    ambient = ambient*Ka;
    
    
    return  (vec3(0.0,0.0,0.0) + diffuse + specular);

}

vec3 color(const ray& r, const hitable_list *world, const camera &cam){
    hit_record rec;
    if(world->hit(r,0.00000001,FLT_MAX,rec)){ // se acertar algum objeto da imagem, entra nesse if
        return phong(rec,cam,world);
    }
    else{
        return vec3(0.0,0.0,0.0);
    }
}


int main(){
    int W = 500; // tamanho horizontal da tela
    int H = 500; // tamanho vertical da tela
    int ns = 10; // precisão do antialiasing
    camera cam(vec3(-3.0,3.0,-3.0), vec3(0.0,0.0,0.0), vec3(0.0,1.0,0.0), 90, float(W)/float(H) , 2.0,0.7);//inicializacao qualquer por causa de erro de compilacao
 
    fstream cena;
    cena.open("cenaze.txt");//arquivo descricao
    ofstream out("ze.ppm");//arquivo resultado

    string action;
    map<string,phongMaterial> material_dictionary;
    vector<sphere> objects;
    vector<phongLight> lights;
    while(cena >> action){
        if(action == "res"){
            cena >> H >> W;
        }else if(action == "camera"){
            float px,py,pz,tx,ty,tz,ux,uy,uz,fov,aperture,dist;
            cena >> px >> py >> pz >> tx >> ty >> tz >> ux >> uy >> uz >> fov >>aperture/* >> f*/;//ver esse f, provavel ser o depth of field
            // camera: 1 parametro é a posição da camera, segundo é o alvo, terceiro é o vetor up, quarto é o fov (vertical), quinto é o aspect/ratio
            vec3 direction(tx-px,ty-py,tz-pz);
            dist = direction.size();
            cam = camera(vec3(px,py,pz), vec3(tx,ty,tz), vec3(ux,uy,uz), fov, float(W)/float(H), aperture ,dist);
        }else if(action == "material"){
            float r, g, b, kd, ks, ka, alpha;
            string name; 
            cena>> name >> r >> g >> b >> ka >> kd >> ks >> alpha;
            material_dictionary[name] = phongMaterial(vec3(r,g,b),ka,kd,kd,alpha);
        }else if(action == "sphere"){
            float cx, cy, cz, radius;
            string materialname;
            cena >> cx >> cy >> cz >> radius >> materialname;
            objects.push_back(sphere(vec3(cx,cy,cz), radius, material_dictionary[materialname])); 
        }else if(action == "light"){
            float r, g, b,px,py,pz;
            cena >> r >> g >> b >> px >> py >> pz;
            lights.push_back(phongLight(vec3(r,g,b), vec3(px,py,pz)));
        }else if(action == "planelight"){

        }
    }

    int QUANTIDADE = objects.size();//tamanho do vector de esferas 
    int numLights = lights.size();

    cout<<QUANTIDADE<<" esferas"<<endl;
    cout<<material_dictionary.size()<<" materiais"<<endl;
    cout<<numLights<<" luzes"<<endl;

    hitable *list[QUANTIDADE]; // array de objetos na imagem (inicializar com as esferas)
    for(int i=0;i<QUANTIDADE;i++){
        sphere aux  = objects[i];
        list[i]=new sphere(aux.center, aux.radius, aux.material);
    }

    phongLight lightList[numLights];
    for(int i=0;i<numLights;i++){
        phongLight aux  = lights[i];
        lightList[i]= phongLight(aux.color, aux.position);
    }
    plane lightplane(vec3(0,2,0),vec3(1,2,1),vec3(2,2,2));
    hitable_list *world = new hitable_list(list,QUANTIDADE,lightList,numLights,lightplane); // objeto que tem todas as imagens


   
    out<<"P3"<<'\n'<<W<<'\n'<<H<<'\n'<<"255"<<'\n'; 
    for(int j = H-1; j >= 0; j--){ // começa a preencher a imagem de cima para baixo
        for(int i = 0; i < W; i++){ // e da esquerda para a direita
            vec3 col(0,0,0); 
            for(int s = 0;s < ns;s++) { // for do anti-aliasing: recomendo ler sobre para entender
                float u = float(i + drand48()/*random_digit()*/) / float(W);
                float v = float(j + drand48()/*random_digit()*/) / float(H);
                ray r = cam.get_ray(u,v);
                col += color(r,world,cam);
            }
            col /= float(ns);
            col = vec3(col.x,col.y,col.z); // serve pra ajustar a gamma de cores para visualizadores de imagem
            int ir = int(255.99*col.x);  // vermelho do pixel
            int ig = int(255.99*col.y); // verde do pixel
            int ib = int(255.99*col.z); // azul do pixel
            out<<ir<<" "<<ig<<" "<<ib<<"\n";
        }
    }

} 