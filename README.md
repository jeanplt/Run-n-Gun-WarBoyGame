# Run-n-Gun-WarBoyGame
Este repositório contém um programa modular em C de um jogo do estilo "Run 'n Gun" feito com Allegro, este jogo contém um personagem principal, dois tipos de inimigos (genérico e boss), uma fase e três níveis de dificuldade.

# Jogatina
O jogo inicia com o personagem principal (controlado pelo player) no canto esquerdo do mapa (representado em coordenados pelo arquivo Map.txt), e o objetivo para o player é controlar seu personagem em direção ao canto direito do mapa, enfrentando nesse caminho inimigos (que podem ter mais ou menos vida e velocidade de disparo dependendo da dificuldade), até chegar no chefe e então tentar derrotá-lo. Se o player conseguir derrotar o chefe, o jogo termina e o player sai como vencedor, se o player perder para o chefe ou algum dos demais inimigos, teremos um "Game Over" como derrota para o player.

# Funcionalidades do jogo

O jogo desenvolvido neste repositório possui algumas características básicas que foram avaliadas durante sua apresentação, sendo elas:
- Jogo singleplayer;
- Menu inicial, permitindo iniciar ou sair do jogo;
- Tela de fim de jogo, indicando se o final foi alcançado com sucesso ou não (game over).

Em relação ao personagem principal:
- Sprite 2D (imagem) em pé;
- Sprite 2D (imagem) abaixado;
- Sprite 2D (imagem) pulando;
- Sprite 2D (imagem) andando em pé;
- Sprite 2D (imagem) atirando;
- Mobilidade geral (andar e atirar para todos os lados, assim como pulando);
- Ataque com uma arma (de projétil - o projétil deve ter uma sprite).

Em relação à batalha, temos as seguintes funcionalidades:
- Sistema de pontos de vida que reduz progressivamente conforme os golpes recebidos por um personagem.

Em relação aos cenários:
- Um cenário com background de imagem;
- Rolling background (o cenário muda conforme o jogador se movimenta).

Em relação aos inimigos:
- Um tipo de inimigo normal, que lança projéteis, com suas respectivas sprites, ataque e movimentação;
- Um inimigo chefe, diferente do inimigo normal, com suas respectivas sprites, ataque e movimentação.

Em relação à fase:
- Uma fase;
- 6 inimigos espalhados pela fase (pode ser alterado pela quantidade de 3's presentes em Map.txt);
- 1 chefe que deve aparecer após a derrota dos 6 inimigos (pode ser alterado pela quantidade de 4's presentes em Map.txt).

Funcionalidades extras:
- Função de “abaixar” e “atirar abaixado” tanto para o personagem principal quanto para o chefe.
- Dificuldade ajustável no menu de opções (Fácil, Médio e Díficil).

