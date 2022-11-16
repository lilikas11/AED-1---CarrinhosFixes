//
// AED, August 2022 (Tomás Oliveira e Silva)
//
// First practical assignement (speed run)
//
// Compile using either
//   cc -Wall -O2 -D_use_zlib_=0 solution_speed_run.c -lm
// or
//   cc -Wall -O2 -D_use_zlib_=1 solution_speed_run.c -lm -lz
//
// Place your student numbers and names here
//   N.Mec. XXXXXX  Name: XXXXXXX
//

//
// static configuration
//

#define _max_road_size_ 800 // the maximum problem size
#define _min_road_speed_ 2  // must not be smaller than 1, shouldnot be smaller than 2
#define _max_road_speed_ 9  // must not be larger than 9 (only because of the PDF figure)

//
// include files --- as this is a small project, we include the PDF generation code directly from make_custom_pdf.c
//

#include <math.h>
#include <stdio.h>
#include "../P02/elapsed_time.h"
#include "make_custom_pdf.c"

//
// road stuff
//

static int max_road_speed[1 + _max_road_size_]; // positions 0.._max_road_size_

static void init_road_speeds(void)
{
  double speed;
  int i;

  for (i = 0; i <= _max_road_size_; i++)
  {
    speed = (double)_max_road_speed_ * (0.55 + 0.30 * sin(0.11 * (double)i) + 0.10 * sin(0.17 * (double)i + 1.0) + 0.15 * sin(0.19 * (double)i));
    max_road_speed[i] = (int)floor(0.5 + speed) + (int)((unsigned int)random() % 3u) - 1;
    if (max_road_speed[i] < _min_road_speed_)
      max_road_speed[i] = _min_road_speed_;
    if (max_road_speed[i] > _max_road_speed_)
      max_road_speed[i] = _max_road_speed_;
  }
}

//
// description of a solution
//

typedef struct
{
  int n_moves;                        // the number of moves (the number of positions is one more than the number of moves)
  int positions[1 + _max_road_size_]; // the positions (the first one must be zero)
} solution_t;

//
// the (very inefficient) recursive solution given to the students
//

static solution_t solution_1, solution_1_best;
static double solution_1_elapsed_time; // time it took to solve the problem
static unsigned long solution_1_count; // effort dispended solving the problem

static void solution_1_recursion(int move_number, int position, int speed, int final_position)
{
  int i, new_speed;

  // record move
  solution_1_count++;
  solution_1.positions[move_number] = position;
  // is it a solution?
  if (position == final_position && speed == 1)
  {
    // is it a better solution?
    if (move_number < solution_1_best.n_moves)
    {
      solution_1_best = solution_1;
      solution_1_best.n_moves = move_number;
    }
    return;
  }
  // no, try all legal speeds
  for (new_speed = speed - 1; new_speed <= speed + 1; new_speed++)
    if (new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for (i = 0; i <= new_speed && new_speed <= max_road_speed[position + i]; i++)
        ;
      if (i > new_speed)
        solution_1_recursion(move_number + 1, position + new_speed, new_speed, final_position);
    }
}

// minha
static void solution_2v1_recursionTeste(int move_number, int position, int speed, int final_position)
{
  int i, new_speed;

  // record move
  solution_1_count++;
  solution_1.positions[move_number] = position;
  // is it a solution?
  if (position == final_position && speed == 1)
  {

    solution_1_best = solution_1;
    solution_1_best.n_moves = move_number;

    return;
  }
  // no, try all legal speeds
  for (new_speed = speed + 1; new_speed >= speed - 1; new_speed--)
    if (new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for (i = 0; i <= new_speed && new_speed <= max_road_speed[position + i]; i++)
        ;
      if (i > new_speed)
        solution_1_recursion(move_number + 1, position + new_speed, new_speed, final_position);
      // if (solution_1_best == 0)
      //   break;
    }

  return;
}

static void solution_2v2_GoBack(int move_number, int position, int speed, int final_position)
{
  // o intuito desta funcao é trablhar com o move_number e o solution_1_best.positions[move_number] = position;
  // ou seja vamos adicionando +1 ao move_number e a posicao relativa ao solution_1_best.positions[move_number] = position;
  // se der problema (condicoes 1,2,3); move_number -1 e reescrevemos as posicoes em solution_1_best.positions[move_number] = position;

  int new_speed, brk;
  int brks[801] = {0};
  int speeds[801] = {0};
  int positions[801] = {0};

  while (position < final_position) // aqui começamos a trabalhar com os moves
  {
    printf("----------------------------------------------------\n");
    printf("NOVO CICLO\n");
    printf("Move_number=%d\n", move_number);
    brk = brks[move_number];
    speed = speeds[move_number - 1];
    position = positions[move_number - 1];
    printf("brk=%d\n", brk);
    printf("speed=%d\n", speed);
    printf("position=%d\n", position);

    if (brk == 0 && speed + 1 >= 1 && speed + 1 <= _max_road_speed_ && position + speed + 1 <= final_position) // --aumentar a speed-- o algoritmo é ganancioso, primeiro tenta avancar o maximo possivel
    {

      printf("aumentar\n");
      new_speed = speed + 1;
      for (int i = 1; i <= new_speed; i++)
      {
        if (new_speed > max_road_speed[position + i])
        {
          printf("Não consegui aumentar\n");
          brk++;
          break; // aumentar a speed deu errrado, entao tenta manter a speed
        }
      }
    }

    if (brk == 1 && speed >= 1 && speed <= _max_road_speed_ && position + speed <= final_position) // --manter a speed--
    {
      printf("manter\n");
      new_speed = speed;
      for (int i = 0; i <= new_speed; i++)
      {
        if (new_speed <= max_road_speed[position + i])
        {
          printf("Não consegui manter\n");
          brk++;
          break; // aumentar a speed deu errado, entao tenta diminuir a speed
        }
      }
    }
    if (brk == 2 && speed - 1 >= 1 && speed - 1 <= _max_road_speed_ && position + speed - 1 <= final_position) //--diminui a speed--
    {
      printf("diminuir\n");
      new_speed = speed - 1;
      for (int i = 0; i <= new_speed; i++)
      {
        if (new_speed <= max_road_speed[position + i])
        {
          printf("Não consegui diminuir\n");
          brk++;
          break; // aumentar a speed deu errrado, entao vai para a iteração
        }
      }
    }
    if (brk >= 3) //--refaz o move anterior
    {
      printf("andar para trás\n");
      move_number = move_number - 1; //????? -1 ou -2 ???????
      brk = brks[move_number] + 1;
      brks[move_number] = brk;
      break;
    }

    printf("DEU CERTO\n");

    speed = new_speed;
    position += speed;
    solution_1_count++;
    solution_1_best.positions[move_number] = position;
    brks[move_number] = brk;
    positions[move_number] = position;
    speeds[move_number] = speed;
    move_number++;
    printf("brk=%d\n", brk);
    printf("speed=%d\n", speed);
    printf("position=%d\n", position);
  }
}

static void solution_3_SmartWay(int move_number, int position, int speed, int final_position)
{
  int speed_test, md_position, new_speed, position_test, brk;
  solution_1_best.n_moves = 0;

  while (position < final_position)
  {

    for (new_speed = speed + 1; new_speed >= speed - 1; new_speed--)
    {

      if (new_speed > _max_road_speed_) // testa se não ultrapassa o final TODO: n(n+1)2
      {
        continue;
      }

      position_test = position;

      for (speed_test = new_speed; speed_test >= 1; speed_test--) // i percorre todos os valores desde speed+1 (movimento ideal) até 1... --> velocidades assumidas na travagem
      {
        brk = 0;

        if ((position_test + speed_test) > final_position)
        {
          brk = 1;
          break;
        }
        for (md_position = 1; md_position <= speed_test; md_position++) // testar posicao de travagem
        {
          if (speed_test > max_road_speed[position_test + md_position]) // passa a velocidade da casa?
          {
            brk = 1;
            break;
          }
        }

        if (brk)
        {
          break; // deu errado? stop.
        }

        position_test += speed_test; // future_position --> posicao teste, i --> speed teste
      }

      if (!brk) // não deu errado? temos o proximo passo :) --> atualizar as variaveis
      {
        speed = new_speed;
        position += speed;
        solution_1_count++;
        solution_1_best.positions[move_number++] = position;
        solution_1_best.n_moves++;
        break;
      }
    }
  }
}

// static void solution_4_alpes(int move_number, int position, int speed, int final_position)
// {
//   int max, m_vel, new_speed,
//   max_road_speed

//   // record move
//   solution_1_count++;
//   solution_1.positions[move_number] = position;
//   // is it a solution?
//   if (position == final_position && speed == 1)
//   {

//     solution_1_best = solution_1;
//     solution_1_best.n_moves = move_number;

//     return;
//   }
//   // no, try all legal speeds
//   for (new_speed = speed + 1; new_speed >= speed - 1; new_speed--)
//     if (new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
//     {
//       for (i = 0; i <= new_speed && new_speed <= max_road_speed[position + i]; i++)
//         ;
//       if (i > new_speed)
//         solution_1_recursion(move_number + 1, position + new_speed, new_speed, final_position);
//       // if (solution_1_best == 0)
//       //   break;
//     }

//   return;
// }

static void solution_5_BestWay(int move_number, int position, int speed, int final_position)
{
  int speed_test, md_position, new_speed, position_test, brk, fp;
  brk = 0; // vai nos dizer se encontrou a solução
  fp = 0;  // vai nos dizer se já está a chegar à posição final
  solution_1_best.n_moves = 0;
  printf("mv: %d, pos: %d, spd: %d, fp: %d \n", move_number, position, speed, final_position);
  while (position >= 0 && position < final_position)
  { // printf("mv: %d, pos: %d, spd: %d, fp: %d \n", move_number, position, speed, final_position);

    // printf("speed %d, postion %d movenumber %d\n", speed, position, move_number);
    if (fp == 2) // chegou à posição final entao só diminui a velocidade
    {

      if (speed != 1)
        speed--;
      position += speed;
      solution_1_count++;
      solution_1_best.positions[move_number++] = position;
      solution_1_best.n_moves++;
    }
    else
    {
      if (fp == 1)
      {
        // resetamos a variavel
        for (speed_test = speed; speed_test >= 1; speed_test--) // i percorre todos os valores desde speed+1 (movimento ideal) até 1... --> velocidades assumidas na travagem
        {

          // neste for ele começa no speed_test para não testar novamente as speeds anteriores

          if ((position_test + speed_test) > final_position)
          {

            fp = 2; // a partir de agora ele só vai diminuir, por isso entra na condição fp
            brk = 1;
            break;
          }
          for (md_position = 1; md_position <= speed_test; md_position++) // testar posicao de travagem
          {

            if (speed_test > max_road_speed[position_test + md_position]) // passa a velocidade da casa?
            {
              brk = 1;
              break;
            }
          }

          if (brk)
          {

            break; // deu errado? stop.
          }

          position_test += speed_test; // future_position --> posicao teste, i --> speed teste
        }
        if (!brk) // chegou aqui porque deu errado ou certo?
        {         // deu certo, guarda as variaveis e faz a proxima iteração
          speed = new_speed;
          position += speed;
          solution_1_count++;
          solution_1_best.positions[move_number++] = position;
          solution_1_best.n_moves++;
        }
        else
        {
          new_speed--; // DIMINUIR a velocidade, se não der mais nenhum ele vai obrigatoriamente diminuir a velocidade, por isso nao precisa de testar todas as outras condições
          speed = new_speed;
          position += speed;
          solution_1_count++;
          solution_1_best.positions[move_number++] = position;
          solution_1_best.n_moves++;
        }
      }
      else
      {

        new_speed = speed + 1; // AUMENTAR a velocidade
        if (new_speed <= _max_road_speed_)
        {
          position_test = position;

          for (speed_test = new_speed; speed_test >= 1; speed_test--) // i percorre todos os valores desde speed+1 (movimento ideal) até 1... --> velocidades assumidas na travagem
          {                                                           // TODO: experimentar isto com spt >= 2

            if ((position_test + speed_test) > final_position) // testa se a posição não passa a posicao final
            {
              fp = 1;
              brk = 1;
              break; // sim? então tenta manter a velocidade
            }
            for (md_position = 1; md_position <= speed_test; md_position++) // testar posicao de travagem
            {
              if (speed_test > max_road_speed[position_test + md_position]) // passa a velocidade da casa?
              {
                brk = 1; // sim? então tenta manter a velocidade
                break;
              }
            }
            if (brk)
            {
              break; // deu errado? stop. tenta manter
            }

            position_test += speed_test; // future_position --> posicao teste, i --> speed teste
          }
        }
        else
        {
          brk = 1;
        }
        // printf("brk: %d /brk", brk);
        if (!brk) // chegou aqui porque deu errado ou certo?
        {         // deu certo, guarda as variaveis e faz a proxima iteração

          speed = new_speed;
          position += speed;
          solution_1_count++;
          solution_1_best.positions[move_number++] = position;
          solution_1_best.n_moves++;
          // printf("move: %d /move", move_number);
        }
        else
        {
          // se não deu para aumentar tenta MANTER a velocidade
          new_speed--;
          brk = 0;                                        // resetamos a variavel
          for (speed_test; speed_test >= 1; speed_test--) // i percorre todos os valores desde speed+1 (movimento ideal) até 1... --> velocidades assumidas na travagem
          {                                               // neste for ele começa no speed_test para não testar novamente as speeds anteriores

            if ((position_test + speed_test) > final_position)
            {
              fp = 1; // a partir de agora ele só vai diminuir, por isso entra na condição fp
              brk = 1;
              break;
            }
            for (md_position = 1; md_position <= speed_test; md_position++) // testar posicao de travagem
            {
              if (speed_test > max_road_speed[position_test + md_position]) // passa a velocidade da casa?
              {
                brk = 1;
                break;
              }
            }

            if (brk)
            {
              break; // deu errado? stop.
            }
            position_test += speed_test; // future_position --> posicao teste, i --> speed teste
          }
          if (!brk) // chegou aqui porque deu errado ou certo?
          {         // deu certo, guarda as variaveis e faz a proxima iteração
            speed = new_speed;
            position += speed;
            solution_1_count++;
            solution_1_best.positions[move_number++] = position;
            solution_1_best.n_moves++;
          }
          else
          {
            if(speed != 1) new_speed--; // DIMINUIR a velocidade, se não der mais nenhum ele vai obrigatoriamente diminuir a velocidade, por isso nao precisa de testar todas as outras condições
            speed = new_speed;
            position += speed;
            solution_1_count++;
            solution_1_best.positions[move_number++] = position;
            solution_1_best.n_moves++;
          }
        }
      }
    }
  }
}

// minha
static void solve_1(int final_position)
{
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_1: bad final_position\n");
    exit(1);
  }
  solution_1_elapsed_time = cpu_time();
  solution_1_count = 0ul;
  solution_1_best.n_moves = final_position + 100;
  solution_5_BestWay(0, 0, 0, final_position); // mudei
  solution_1_elapsed_time = cpu_time() - solution_1_elapsed_time;
}

//
// example of the slides
//

static void example(void)
{
  int i, final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_1(final_position);
  make_custom_pdf_file("example.pdf", final_position, &max_road_speed[0], solution_1_best.n_moves, &solution_1_best.positions[0], solution_1_elapsed_time, solution_1_count, "Plain recursion");
  printf("mad road speeds:");
  for (i = 0; i <= final_position; i++)
    printf(" %d", max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for (i = 0; i <= solution_1_best.n_moves; i++)
    printf(" %d", solution_1_best.positions[i]);
  printf("\n");
}

//
// main program
//

int main(int argc, char *argv[argc + 1])
{
#define _time_limit_ 3600.0
  int n_mec, final_position, print_this_one;
  char file_name[64];

  // generate the example data
  if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'e' && argv[1][2] == 'x')
  {
    example();
    return 0;
  }
  // initialization
  n_mec = (argc < 2) ? 0xAED2022 : atoi(argv[1]);
  srandom((unsigned int)n_mec);
  init_road_speeds();
  // run all solution methods for all interesting sizes of the problem
  final_position = 1;
  solution_1_elapsed_time = 0.0;
  printf("    + --- ---------------- --------- +\n");
  printf("    |                plain recursion |\n");
  printf("--- + --- ---------------- --------- +\n");
  printf("  n | sol            count  cpu time |\n");
  printf("--- + --- ---------------- --------- +\n");
  while (final_position <= _max_road_size_ /* && final_position <= 20*/)
  {
    print_this_one = (final_position == 10 || final_position == 20 || final_position == 50 || final_position == 100 || final_position == 200 || final_position == 400 || final_position == 800) ? 1 : 0;
    printf("%3d |", final_position);
    // first solution method (very bad)
    if (solution_1_elapsed_time < _time_limit_)
    {
      solve_1(final_position);
      if (print_this_one != 0)
      {
        sprintf(file_name, "%03d_1.pdf", final_position);
        make_custom_pdf_file(file_name, final_position, &max_road_speed[0], solution_1_best.n_moves, &solution_1_best.positions[0], solution_1_elapsed_time, solution_1_count, "Plain recursion");
      }
      printf(" %3d %16lu %9.3e |", solution_1_best.n_moves, solution_1_count, solution_1_elapsed_time);
    }
    else
    {
      solution_1_best.n_moves = -1;
      printf("                                |");
    }
    // second solution method (less bad)
    // ...

    // done
    printf("\n");
    fflush(stdout);
    // new final_position
    if (final_position < 50)
      final_position += 1;
    else if (final_position < 100)
      final_position += 5;
    else if (final_position < 200)
      final_position += 10;
    else
      final_position += 20;
  }
  printf("--- + --- ---------------- --------- +\n");
  return 0;
#undef _time_limit_
}
