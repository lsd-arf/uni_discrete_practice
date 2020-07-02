#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// вывод массива циклов
void show_fac(int ***fac, int num_vert, int num_cycl)
{
    for (int k = 0; k < num_cycl; k++)
    {
        for (int i = 0; i < num_vert; i++)
        {
            for (int j = 0; j < num_vert; j++)
                cout << fac[k][i][j] << ' ';
            cout << '\n';
        }
        cout << '\n';
    }
}

// инициализация массива циклов
void create_fac(int num_vert, int num_cycl, int ***fac, string fac_name)
{
    ifstream fin(fac_name);
    int index = -1; // индекс цикла в трёхмерном массиве
    int v1 = 0;     // вершины,
    int v2 = 0;     // связывающие рёбра

    // обнуляем
    for (int k = 0; k < num_cycl; k++)
    {
        fac[k] = new int *[num_vert];
        for (int i = 0; i < num_vert; i++)
        {
            fac[k][i] = new int[num_vert];
            for (int j = 0; j < num_vert; j++)
                fac[k][i][j] = 0;
        }
    }

    // заполняем массив циклами
    while (!fin.eof())
    {
        fin >> v1 >> v2;
        if (v1 == -1 && v2 == -1)
            index++;
        else
        {
            fac[index][v1][v2] = 1;
            fac[index][v2][v1] = 1;
        }
    }

    fin.close();
}

// рекурсия, определяющая компоненты суммы
void func_fac(int index, int *visited, int **sum, int **cycle, int ***fac, int num_vert, int num_cycl, bool &stop)
{
    // для проверки суммы на равенство заданному циклу
    bool check_sum = true;

    // добавляем цикл в сумму
    for (int i = 0; i < num_vert; i++)
        for (int j = 0; j < num_vert; j++)
            if (sum[i][j] == 1 && fac[index][i][j] == 1)
                sum[i][j] = 0;
            else
                sum[i][j] += fac[index][i][j];

    // посещаем цикл при вызове рекурсии
    visited[index] = 1;

    // если найден неравный элемент, отмечаем, что матрицы неравны
    for (int i = 0; i < num_vert && check_sum; i++)
        for (int j = 0; j < num_vert && check_sum; j++)
            if (cycle[i][j] != sum[i][j])
                check_sum = false;

    // если true - завершаем рекурсию, иначе идём дальше
    if (check_sum)
        stop = true;
    else
    {
        if (index + 1 != num_cycl) // если не дно рекурсии
            for (int i = index + 1; i < num_cycl && !stop; i++)
                func_fac(i, visited, sum, cycle, fac, num_vert, num_cycl, stop);

        if (!stop) // если рекурсия не завершена
        {
            // вычитаем текущий цикл из суммы
            for (int i = 0; i < num_vert; i++)
                for (int j = 0; j < num_vert; j++)
                    if (sum[i][j] == 1 && fac[index][i][j] == 1)
                        sum[i][j] = 0;
                    else
                        sum[i][j] += fac[index][i][j];
            visited[index] = 0;
        }
    }
}

// вывод матрицы
void show_matrix(int num_vert, int **matrix)
{
    for (int i = 0; i < num_vert; i++)
    {
        for (int j = 0; j < num_vert; j++)
            cout << matrix[i][j] << ' ';
        cout << endl;
    }
}

// красивое представление суммы циклов
void beauty_view(int **cycle, int ***fac, int num_vert, int num_cycl, int sym_position, int *visited, int last_true)
{
    for (int i = 0; i < num_vert; i++)
    {
        // строка исходного цикла
        for (int j = 0; j < num_vert; j++)
            cout << cycle[i][j] << ' ';

        // знак = посередине
        if (i == sym_position)
            cout << "= ";
        else
            cout << "  ";

        // строки компонентов суммы
        for (int k = 0; k < num_cycl; k++)
            if (visited[k] == 1)
            {
                for (int j = 0; j < num_vert; j++)
                    cout << fac[k][i][j] << ' ';

                // знак + посередине
                if (i == sym_position && k != last_true)
                    cout << "+ ";
                else
                    cout << "  ";
            }

        cout << endl;
    }
}

int main()
{
    system("cls");
    string cycle_name = "for10/c1.txt"; // цикл
    string fac_name = "for10/fac.txt";  // фмц
    string graph_name = "for10/g.txt";  // граф
    /*cout << "Cycle name --> ";
    cin >> cycle_name;
    cout << "FAC name --> ";
    cin >> fac_name;
    cout << "Graph name --> ";
    cin >> graph_name;*/
    int num_vert;      // количество вершин графа
    int num_cycl = 0;  // количество циклов (на основе количества рёбер графа num_cycl-num_vert+1)
    bool stop = false; // флаг для остановки рекурсии
    int sym_position;  // позиция знаков + и = при сложении матриц
    int last_true;     // позиция крайнего компонента суммы, чтобы не писать крайний + в beauty_view

    /* ---------- БЛОК ГРАФА ---------- */
    ifstream fs_graph(graph_name);
    fs_graph >> num_vert;
    sym_position = (num_vert - 1) / 2; // средняя (при нечётном num_vert) или околосредняя (при чётном num_vert)
    int **graph = new int *[num_vert]; // граф
    int v1_graph, v2_graph;            // вершины

    // обнуляем граф
    for (int i = 0; i < num_vert; i++)
    {
        graph[i] = new int[num_vert];
        for (int j = 0; j < num_vert; j++)
            graph[i][j] = 0;
    }

    // заполняем граф
    while (!fs_graph.eof())
    {
        fs_graph >> v1_graph;
        fs_graph >> v2_graph;
        graph[v1_graph][v2_graph] = 1;
        graph[v2_graph][v1_graph] = 1;
        num_cycl++;
    }

    fs_graph.close();
    num_cycl = num_cycl - num_vert + 1;

    /* ---------- БЛОК ЦИКЛА ---------- */
    ifstream fs_cycle(cycle_name);
    int **cycle = new int *[num_vert]; // цикл
    int v1_cycle, v2_cycle;            // вершины

    // обнуляем цикл
    for (int i = 0; i < num_vert; i++)
    {
        cycle[i] = new int[num_vert];
        for (int j = 0; j < num_vert; j++)
            cycle[i][j] = 0;
    }

    // заполняем цикл
    while (!fs_cycle.eof())
    {
        fs_cycle >> v1_cycle;
        fs_cycle >> v2_cycle;
        cycle[v1_cycle][v2_cycle] = 1;
        cycle[v2_cycle][v1_cycle] = 1;
    }
    fs_cycle.close();

    /* ---------- БЛОК МАССИВА ЦИКЛОВ И СУММЫ ---------- */
    int ***fac = new int **[num_cycl]; // массив фмц
    int *visited = new int[num_cycl];  // циклы, вошедшие в сумму
    int **sum = new int *[num_vert];   // матрица суммы

    // ставим как непосещённые
    for (int i = 0; i < num_cycl; i++)
        visited[i] = 0;

    // обнуляем сумму
    for (int i = 0; i < num_vert; i++)
    {
        sum[i] = new int[num_vert];
        for (int j = 0; j < num_vert; j++)
            sum[i][j] = 0;
    }

    create_fac(num_vert, num_cycl, fac, fac_name);
    cout << "Cycle number --> " << num_cycl << "\n\n";

    /* ---------------------------------------- */

    for (int i = 0; i < num_cycl && !stop; i++)
        func_fac(i, visited, sum, cycle, fac, num_vert, num_cycl, stop);

    cout << "***** Cycles for sum *****";
    cout << "\n----------------------------------------\n";
    for (int i = 0; i < num_cycl; i++)
        cout << visited[i] << ' ';
    cout << "\n----------------------------------------\n";

    for (int i = num_cycl - 1; i >= 0; i--)
        if (visited[i] == 1)
        {
            last_true = i;
            break;
        }

    /* ---------------------------------------- */

    cout << '\n';
    cout << "***** View of cycles sum *****\n";
    cout << "----------------------------------------\n";
    beauty_view(cycle, fac, num_vert, num_cycl, sym_position, visited, last_true);
    cout << "----------------------------------------\n";

    // очищаем динамическую память
    delete graph;
    delete cycle;
    delete fac;
    delete visited;
    delete sum;
}