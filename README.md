# PSI3422 - Laboratório de sistemas eletrônicos
Repositório da disciplina de Laboratório de sistemas eletrônicos da Escola Politecnica da USP

Na segunda metade do curso, o objetivo é realizar o projeto de um carrinho autônomo que segue placas e de acordo com dígitos manuscritos dentro da placa, realizar uma ação correspondente.

O carrinho é composto de uma Raspberry Pi, uma câmera, duas pontes-H, dois motores, bateria de alimentação e outros componentes como jumpers e cabos para as ligaçoes.

O princípio geral de funcionamento é o seguinte: há o servidor (raspberry Pi) e o cliente (computador), o servidor captura imagens através da câmera e envia para o cliente através de comunicação TCP/IP, o cliente realiza todo o processamento da imagem (reconhecimento da placa, reconhecimento dos dígitos) e envia comandos para o servidor de acordo com o que foi processado da imagem recebida, o servidor recebe os dados e envia os comandos para o GPIO para o funcionamento do carrinho.

Neste projeto são abordados os seguintes tópicos de sistemas eletrônicos:

- Comunicação TCP/IP
- Placa com sistema embarcado Linux
- Processamento de imagens e Visão Computacional
- Aprendizagem de máquina
- Processamento paralelo
- Máquina de Estados Finita
- Prática de Programação Orientada a Objetos

A linguagem de programaço utilizada foi C++ e as bibliotecas OpenCV, [Cekeikon](http://www.lps.usp.br/hae/software/)(de autoria do Professor), Wiring Pi(para uso do GPIO da raspberry).


