#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <iomanip>


using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono;
using namespace std;

static const char alphanum[] =
        "abc";

string gen_random(int len) {

    string tmp_s;

    srand( (unsigned) time(NULL) * getpid());

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];


    return tmp_s;

}

struct PRNG
{
    std::mt19937 engine;
};

void initGenerator(PRNG& generator)
{
    // Используем время с 1 января 1970 года в секундах как случайное зерно
    const unsigned seed = unsigned(std::time(nullptr));
    generator.engine.seed(seed);
}

// Генерирует индекс в диапазоне [0, size)
size_t random_index(size_t text_len, int pat_len)
{
    PRNG generator;
    initGenerator(generator);

    // Создаём распределение
    std::uniform_int_distribution<size_t> distribution(0, text_len - pat_len);

    // Вычисляем псевдослучайное число: вызовем распределение как функцию,
    //  передав генератор произвольных целых чисел как аргумент.
    return distribution(generator.engine);
}

vector<string> get_text_and_pattern(int length_text = 10, int length_pattern = 2)
{
    string text = gen_random(length_text);
    int random_indx = random_index(length_text, length_pattern);

//    cout << "random_index =  " << random_indx << endl;
    string pat = text.substr(random_indx, length_pattern-1)+ "2";

    sleep_until(system_clock::now() + seconds(1));

    random_indx = random_index(length_text, length_pattern);
//    cout << "2 random_index =  " << random_indx << endl;
    text.replace(random_indx, pat.length(), pat);

    return vector<string>{text, pat};

}

string::size_type KMP(string& S, string& pattern)
{
    vector<int> pf (pattern.length());
    pf[0] = 0;
    for (int k = 0, i = 1; i < pattern.length(); ++i)
    {
        while ((k > 0) && (pattern[i] != pattern[k]))
            k = pf[k-1];

        if (pattern[i] == pattern[k])
            k++;

        pf[i] = k;
    }

    for (int k = 0, i = 0; i < S.length(); ++i)
    {
        while ((k > 0) && (pattern[k] != S[i]))
            k = pf[k-1];

        if (pattern[k] == S[i])
            k++;

        if (k == pattern.length())
            return (i - pattern.length() + 1); // else continue
    }
    return (string::npos);
}


int main()
{
    cout.sync_with_stdio(false);
    vector <int> n_len_of_pattern = {2, 4, 8}; //
    vector <int> m_len_of_text = { 13107200, 26214400, 52428800, 104857600};

    // m строк, n столбцов
    vector <vector <double>> matrix_avg(m_len_of_text.size(), vector <double> (n_len_of_pattern.size(), 0.0) );
    double sum = 0.0; // for 32 elements

    int len_text{0}, len_pat{0};
//    cin >> len_text >> len_pat;
    vector <string> t_p;
    string text, pat;

    int num_step = 32;
    for (int lt{0}; lt < m_len_of_text.size(); lt++)
    {
        for (int lp{0}; lp < n_len_of_pattern.size(); lp++)
        {
            for (int indx{0}; indx < num_step; indx++)
            {
                len_text = m_len_of_text[lt];
                len_pat = n_len_of_pattern[lp];

//                cout << "M N " << len_text << " " << len_pat << endl;

//              generate random pat and text
                t_p = get_text_and_pattern(len_text, len_pat);
                text = t_p[0];
                string pat = t_p[1];

//                cout << text << "  " ;
//                cout << pat << endl;

                auto start =  chrono::high_resolution_clock::now();
//                cout<<"Running " << endl;

                auto match = KMP(text, pat);

                auto end = chrono::high_resolution_clock::now();
                double time_taken =
                        chrono::duration_cast<chrono::nanoseconds>(end - start).count();

                time_taken *= 1e-9;
                sum = sum + time_taken;

//                cout << "Time taken by function: " << fixed << time_taken << " seconds" << endl;
//
//                cout << "index_match = " << match << endl;

                long display_cnt{1};

                //Check for errors, just in case
//                long i = text.find(pat);
//
//                if(i==match)
//                    cout << "ok" << endl;
//                else
//                    cout<<"mismatch " << i <<endl;

                sleep_until(system_clock::now() + seconds(1));
                cout << "\n_______________\n" << endl;
            }
//            cout << sum << " " << sum/num_step << endl;
            matrix_avg[lt][lp] = sum/num_step;
//            cout << m_len_of_text[lt] << " " << n_len_of_pattern[lp] << " : " << matrix_avg[lt][lp] << endl;
            sum = 0.0;
        }
    }

//    вывод матрицы среднего времени
    for (int i = 0; i < m_len_of_text.size(); ++i)
    {
        cout << m_len_of_text[i] << ": ";
        for (int j = 0; j < n_len_of_pattern.size(); ++j)
        {
            cout << matrix_avg[i][j] << ' ';
        }
        cout << endl;
    }

    return 0;
}