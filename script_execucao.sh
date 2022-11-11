#!/bin/bash
echo "Algoritmo converter um objeto 3D em uma imagem normal: --------------------------------------"
echo ""

g++ -o exec cria_imagens_3D_POO.cpp -std=c++11

#objetos 3D para escolha
arq_origem="cara.obj"
#arq_origem="weird.obj"


arq_buffer="buffer.obj"
arq_destino="imagem_gerada.ppm"

#angulos em graus
rotacao_x="20"
rotacao_y="130"
rotacao_z="0"

#ponto central da imagem 2D que vai entrar a imagem 3D
centro_x="300"
centro_y="300"

#escala(escala do objeto 3D na imagem 2D)
escala="230"

#cor do objeto 3D
red_frente="255"
green_frente="0"
blue_frente="0"

#cor do fundo(imagem atras do objeto)
red_fundo="0"
green_fundo="0" 
blue_fundo="255"


echo "Normalização do obejto 3D:"
ctmconv $arq_origem $arq_buffer --no-normals --no-texcoords
arq_origem="sufixo.obj"

#script em python para retirar barras dos arquivos
python3 limpa_lixo_arquivo_obj.py $arq_buffer $arq_origem

./exec $arq_origem $arq_destino $rotacao_x $rotacao_y $rotacao_z $centro_x $centro_y $escala $red_frente $green_frente $blue_frente $red_fundo $green_fundo $blue_fundo

rm $arq_buffer
rm exec
