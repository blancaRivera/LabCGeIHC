#version 330 core

in vec2 out_uv;
out vec4 color;
//unidad de textura de tipo sampler2D
uniform sampler2D ourTexture;

void main(){
	//obtiene el color de la textura con base a la unidad de textura y las coordenadas
	color = texture(ourTexture, out_uv);
	//color = vec4(0.3, 0.5, 0.4, 1.0);
}