#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>


/* include reversible function*/
#include <reversible/circuit.hpp>
#include <reversible/gate.hpp>
#include <reversible/target_tags.hpp>
#include <reversible/truth_table.hpp>
#include <reversible/functions/add_gates.hpp>
#include <reversible/functions/add_circuit.hpp>
#include <reversible/functions/add_line_to_circuit.hpp>
#include <reversible/functions/clear_circuit.hpp>
#include <reversible/functions/circuit_to_truth_table.hpp>
#include <reversible/simulation/simple_simulation.hpp>
#include <reversible/optimization/esop_post_optimization.hpp>
#include <reversible/optimization/simplify.hpp>
#include <reversible/synthesis/esop_synthesis.hpp>
#include <reversible/io/print_circuit.hpp>
#include <reversible/io/read_realization.hpp>
#include <reversible/io/write_realization.hpp>
#include <reversible/io/write_pla.hpp>
#include <reversible/io/write_specification.hpp>
#include <reversible/verification/xorsat_equivalence_check.hpp>
#include <reversible/utils/costs.hpp>

/* include classical function */
#include <classical/optimization/exorcism_minimization.hpp>
#include <classical/optimization/esop_minimization.hpp>

/* include core function */

/* Boost library */
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/iterator_range.hpp>


using namespace std;
using namespace cirkit;

struct Range
{
    unsigned target;
    unsigned range[2];
};


bool isPi4Target(unsigned *pi, unsigned target, unsigned critical_line);
bool isPi4Control(unsigned *pi, gate::control_container control, unsigned critical_line);
bool isXaSet(unsigned* pi, unsigned src, unsigned critical_line);
bool isXbSet(unsigned* rs, unsigned* pt, unsigned src, unsigned critical_line);
bool isXcSet(unsigned* key0, unsigned* key1, unsigned src, unsigned critical_line);

void write_miter_cnf(circuit circ1, circuit circ2);
double check_result();

// main function
void Encryption_original_version(int argc, const char** argv);
void Encryption_new_version(int argc, const char** argv);
void Decryption(int argc, const char** argv);

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        cout << "Type: ./reversible_circuit_2_SFLL-HD_circuit [input.real] [output.real]\n";
        exit(1);
    }
    //Encryption_original_version(argc, argv);
    Encryption_new_version(argc, argv);
    Decryption(argc, argv);

    

    /*----------------Start Reverse Engineering---------------*/

    /*-----------------End Reverse Engineering----------------*/

    return 0;
}

bool isPi4Target(unsigned *pi, unsigned target, unsigned critical_line)
{
    for (int i = 0; i < critical_line; i++)
    {
        if (pi[i] == target)
            return true;
    }
    return false;
}

bool isPi4Control(unsigned *pi, gate::control_container control, unsigned critical_line)
{
    if (control.size() == 0)
        return true;
    unsigned counter = 0;
    for (int i = 0; i < critical_line; i++)
    {
        for (const auto& c: control)
        {
            if (pi[i] == c.line())
                counter++;
        }
    }
    if (counter == control.size())
        return true;
    else
        return false;
}

bool isXaSet(unsigned* pi, unsigned src, unsigned critical_line)
{
    for (unsigned i = 0; i < critical_line; i++)
    {
        if (pi[i] == src)
            return true;
    }
    return false;
}

bool isXbSet(unsigned* rs, unsigned* pt, unsigned src, unsigned critical_line)
{
    if (rs[0] == src)
        return true;
    if (rs[1] == src)
        return true;
    for (unsigned i = 0; i < critical_line; i++)
    {
        if (pt[i] == src)
            return true;
    }
    return false;
}

bool isXcSet(unsigned* key0, unsigned* key1, unsigned src, unsigned critical_line)
{
    for (unsigned i = 0; i < critical_line; i++)
    {
        if (key0[i] == src)
            return true;
    }
    for (unsigned i = 0; i < critical_line; i++)
    {
        if (key1[i] == src)
            return true;
    }
    return false;
}

void write_miter_cnf(circuit circ1, circuit circ2)
{
    fstream outFile("tmp_miter.cnf", ios::out);
    vector<string> str;
    string tmp_str = "";
    unsigned clause_counter = 0;


    if (!outFile)
    {
        cout << "Open tmp_miter.cnf error!\n";
        exit(1);
    }
    
    while (circ1.lines() < circ2.lines())
    {
        tmp_str = "tmpline";
        add_line_to_circuit(circ1, tmp_str, tmp_str);
    }

    vector<unsigned> LastLineId1; // store current line Id in circuit 1
    vector<unsigned> LastLineId2; // store current line Id in circuit 2
    vector<unsigned> pi;
    vector<unsigned> miter;

    unsigned cnfId = 1;
    unsigned tmpId;

    // Set primary input Id
    for (int i = 0; i < circ1.lines(); i++)
    {
        LastLineId1.push_back(cnfId);
        LastLineId2.push_back(cnfId);
        pi.push_back(cnfId++);
    }

    // Construct miter structure
    // Construct circ1's cnf structure
    for (const auto &gate: circ1)
    {
        if (gate.controls().size() == 0)
        { // NOT
            outFile << LastLineId1[gate.targets()[0]] << " " << cnfId << " 0\n";
            outFile << "-" << LastLineId1[gate.targets()[0]] << " -" << cnfId << " 0\n";
            LastLineId1[gate.targets()[0]] = cnfId++;
        }
        else if (gate.controls().size() == 1)
        { // CNOT
            if (gate.controls()[0].polarity())
            { // positive control
                outFile << "-" << LastLineId1[gate.controls()[0].line()] << " -" << LastLineId1[gate.targets()[0]] << " -" << cnfId << " 0\n";
                outFile << LastLineId1[gate.controls()[0].line()] << " " << LastLineId1[gate.targets()[0]] << " -" << cnfId << " 0\n";
                outFile << LastLineId1[gate.controls()[0].line()] << " -" << LastLineId1[gate.targets()[0]] << " " << cnfId << " 0\n";
                outFile << "-" << LastLineId1[gate.controls()[0].line()] << " " << LastLineId1[gate.targets()[0]] << " " << cnfId << " 0\n";
            }
            else
            { // negative control
                outFile << LastLineId1[gate.controls()[0].line()] << " -" << LastLineId1[gate.targets()[0]] << " -" << cnfId << " 0\n";
                outFile << "-" << LastLineId1[gate.controls()[0].line()] << " " << LastLineId1[gate.targets()[0]] << " -" << cnfId << " 0\n";
                outFile << "-" << LastLineId1[gate.controls()[0].line()] << " -" << LastLineId1[gate.targets()[0]] << " " << cnfId << " 0\n";
                outFile << LastLineId1[gate.controls()[0].line()] << " " << LastLineId1[gate.targets()[0]] << " " << cnfId << " 0\n";
            }
            LastLineId1[gate.targets()[0]] = cnfId++;
        }
        else
        { // MCT
            // AND gate's id is tmpId
            tmpId = cnfId++;
            // handle AND
            for (const auto &control: gate.controls())
            {
                if (control.polarity())
                { // positive control
                    outFile << LastLineId1[control.line()] << " -" << tmpId << " 0\n";
                }
                else
                { // negative control
                    outFile << "-" << LastLineId1[control.line()] << " -" << tmpId << " 0\n";
                }
            }
            for (const auto &control: gate.controls())
            {
                if (control.polarity())
                {
                    outFile << "-" << LastLineId1[control.line()] << " ";
                }
                else
                {
                    outFile << LastLineId1[control.line()] << " ";
                }
            }
            outFile << tmpId << " 0\n";
            // handle xor gate
            outFile << "-" << tmpId << " -" << LastLineId1[gate.targets()[0]] << " -" << cnfId << " 0\n";
            outFile << tmpId << " " << LastLineId1[gate.targets()[0]] << " -" << cnfId << " 0\n";
            outFile << tmpId << " -" << LastLineId1[gate.targets()[0]] << " " << cnfId << " 0\n";
            outFile << "-" << tmpId << " " << LastLineId1[gate.targets()[0]] << " " << cnfId << " 0\n";
            LastLineId1[gate.targets()[0]] = cnfId++;
        }
    }
    // Construct circ2's cnf structure
    for (const auto &gate: circ2)
    {
        if (gate.controls().size() == 0)
        { // NOT
            outFile << LastLineId2[gate.targets()[0]] << " " << cnfId << " 0\n";
            outFile << "-" << LastLineId2[gate.targets()[0]] << " -" << cnfId << " 0\n";
            LastLineId2[gate.targets()[0]] = cnfId++;
        }
        else if (gate.controls().size() == 1)
        { // CNOT
            if (gate.controls()[0].polarity())
            { // positive control
                outFile << "-" << LastLineId2[gate.controls()[0].line()] << " -" << LastLineId2[gate.targets()[0]] << " -" << cnfId << " 0\n";
                outFile << LastLineId2[gate.controls()[0].line()] << " " << LastLineId2[gate.targets()[0]] << " -" << cnfId << " 0\n";
                outFile << LastLineId2[gate.controls()[0].line()] << " -" << LastLineId2[gate.targets()[0]] << " " << cnfId << " 0\n";
                outFile << "-" << LastLineId2[gate.controls()[0].line()] << " " << LastLineId2[gate.targets()[0]] << " " << cnfId << " 0\n";
            }
            else
            { // negative control
                outFile << LastLineId2[gate.controls()[0].line()] << " -" << LastLineId2[gate.targets()[0]] << " -" << cnfId << " 0\n";
                outFile << "-" << LastLineId2[gate.controls()[0].line()] << " " << LastLineId2[gate.targets()[0]] << " -" << cnfId << " 0\n";
                outFile << "-" << LastLineId2[gate.controls()[0].line()] << " -" << LastLineId2[gate.targets()[0]] << " " << cnfId << " 0\n";
                outFile << LastLineId2[gate.controls()[0].line()] << " " << LastLineId2[gate.targets()[0]] << " " << cnfId << " 0\n";
            }
            LastLineId2[gate.targets()[0]] = cnfId++;
        }
        else
        { // MCT
            // AND gate's id is tmpId
            tmpId = cnfId++;
            // handle AND
            for (const auto &control: gate.controls())
            {
                if (control.polarity())
                { // positive control
                    outFile << LastLineId2[control.line()] << " -" << tmpId << " 0\n";
                }
                else
                { // negative control
                    outFile << "-" << LastLineId2[control.line()] << " -" << tmpId << " 0\n";
                }
            }
            for (const auto &control: gate.controls())
            {
                if (control.polarity())
                {
                    outFile << "-" << LastLineId2[control.line()] << " ";
                }
                else
                {
                    outFile << LastLineId2[control.line()] << " ";
                }
            }
            outFile << tmpId << " 0\n";
            // handle xor gate
            outFile << "-" << tmpId << " -" << LastLineId2[gate.targets()[0]] << " -" << cnfId << " 0\n";
            outFile << tmpId << " " << LastLineId2[gate.targets()[0]] << " -" << cnfId << " 0\n";
            outFile << tmpId << " -" << LastLineId2[gate.targets()[0]] << " " << cnfId << " 0\n";
            outFile << "-" << tmpId << " " << LastLineId2[gate.targets()[0]] << " " << cnfId << " 0\n";
            LastLineId2[gate.targets()[0]] = cnfId++;
        }
    }
    // Construct xor cnf
    for (unsigned int i = 0; i < LastLineId1.size(); i++)
    {
        outFile << "-" << LastLineId1[i] << " -" << LastLineId2[i] << " -" << cnfId << " 0\n";
        outFile << LastLineId1[i] << " " << LastLineId2[i] << " -" << cnfId << " 0\n";
        outFile << LastLineId1[i] << " -" << LastLineId2[i] << " " << cnfId << " 0\n";
        outFile << "-" << LastLineId1[i] << " " << LastLineId2[i] << " " << cnfId << " 0\n";
        miter.push_back(cnfId++);
    }
    for (const auto& m: miter)
    {
        outFile << m << " ";
    }
    outFile << "0\n";    
    outFile.close();
}

double check_result()
{
    double ece_time;
    system("./programs/minisat tmp_miter.cnf > tmp_result.txt");
    //system("cat tmp_result.txt");
    fstream inFile("tmp_result.txt", ios::in);
    string str;

    if (!inFile)
    {
        cout << "tmp_result.txt can not be opened!\n";
        exit(1);
    }

    while(inFile >> str)
    {
        if (str.compare("CPU") == 0)
        {
            inFile >> str >> str >> str;
            //cout << "hehe: "<< str << endl;
            ece_time = stod(str);
        }
        if (str.compare("UNSATISFIABLE") == 0)
        {
            //cout << "RE circuit and original circuit are equivalent!\n";
            cout << "Equivalence,";
            return ece_time;
        }
        else if (str.compare("SATISFIABLE") == 0)
        {
            cout << "non-equivalence,";
            return ece_time;
        }
    }
    cout << "not found the result\n";
    exit(1);
}

void Encryption_original_version(int argc, const char** argv)
{
    clock_t t, cpuTime;
    t = clock();
    /*-----------------Start encryption----------------*/
    circuit circ, enc_circ;
    string tmp_str;
    tmp_str = argv[1];
    tmp_str = tmp_str.substr(10, tmp_str.size() - 10); // benchmark/
    //tmp_str = tmp_str.substr(21, tmp_str.size() - 21); // revisiting_benchmark/
    tmp_str = tmp_str.substr(0, tmp_str.size() - 5); // *.real
    // Read file begin
    read_realization(circ, argv[1]);
    read_realization(enc_circ, argv[1]);
    //cout << "original circuit:\n" << circ << endl;
    // Read file end
    
    unsigned ori_circuit_line = circ.lines();
    cout << tmp_str << ",";
    cout << ori_circuit_line << ",";
    cout << circ.num_gates() << ",";
    
    while(enc_circ.num_gates())
    {
        enc_circ.remove_gate_at(0);
    }
    
    // add new line for encryption
    
    for (int i = 0; i < ori_circuit_line; i++) // add first key line
    {
        tmp_str = "K" + to_string(i+1) + "1";
        add_line_to_circuit(circ, tmp_str, tmp_str);
        add_line_to_circuit(enc_circ, tmp_str, tmp_str);
    }
    for (int i = 0; i < ori_circuit_line; i++) // add second key line
    {
        tmp_str = "K" + to_string(i+1) + "2";
        add_line_to_circuit(circ, tmp_str, tmp_str);
        add_line_to_circuit(enc_circ, tmp_str, tmp_str);
    }
    tmp_str = "rs1";
    add_line_to_circuit(circ, tmp_str, tmp_str, 0);
    add_line_to_circuit(enc_circ, tmp_str, tmp_str, 0);
    tmp_str = "rs2";
    add_line_to_circuit(circ, tmp_str, tmp_str, 0);
    add_line_to_circuit(enc_circ, tmp_str, tmp_str, 0);
    for (int i = 0; i < ori_circuit_line; i++) // add input pattern line
    {
        tmp_str = "pt" + to_string(i+1);
        add_line_to_circuit(circ, tmp_str, tmp_str, 0);
        add_line_to_circuit(enc_circ, tmp_str, tmp_str, 0);
    }

    // construct restore unit
    unsigned index_counter = 0;
    for (unsigned i = 0; i < ori_circuit_line; i++, index_counter++)
    {
        append_cnot(enc_circ,i, ori_circuit_line + i);
    }
    append_toffoli(enc_circ)(make_var(ori_circuit_line, 0))(3*ori_circuit_line);
    for (unsigned i = 1; i < ori_circuit_line; i++)
    {
        enc_circ[index_counter].add_control(make_var(ori_circuit_line + i, 0));
    }
    index_counter++;
    for (unsigned i = 0; i < ori_circuit_line; i++, index_counter++)
    {
        append_cnot(enc_circ, i, 2*ori_circuit_line + i);
    }
    append_toffoli(enc_circ)(make_var(2*ori_circuit_line, 0))(3*ori_circuit_line + 1);
    for (unsigned i = 1; i < ori_circuit_line; i++)
    {
        enc_circ[index_counter].add_control(make_var(2*ori_circuit_line + i, 0));
    }
    index_counter++;
    for (unsigned i = 0; i < ori_circuit_line; i++, index_counter++)
    {
        append_cnot(enc_circ, i, 3*ori_circuit_line + 2 + i);
    }
    // Find encyption position
    int* enc_pos = new int [ori_circuit_line];
    bool* is_enc = new bool [ori_circuit_line]();
    for (int i = 0; i < ori_circuit_line; i++)
    {
        enc_pos[i] = -1;
    }
    srand(2);
    for (unsigned i = 0; i < circ.num_gates(); i++)
    {
        if (!is_enc[circ[i].targets()[0]])
        {
            is_enc[circ[i].targets()[0]] = true;
            enc_pos[circ[i].targets()[0]] = i;
        }
        else
        {
            if (rand()%2 == 0)
            {
                enc_pos[circ[i].targets()[0]] = i;
            }
        }
    }
    sort(enc_pos, enc_pos+ori_circuit_line);

    int key[2];
    key[0] = rand() % (int(pow(2, ori_circuit_line)) - 2) + 1;
    do
    {
        key[1] = rand() % (int(pow(2, ori_circuit_line)) - 1);
    } while (key[0] == key[1] || key[1] == 0);
    //cout << "key0: " << key[0] << " key1: " << key[1] << endl;

    // insert encryption circuit
    circuit tmp_circ;
    //cout << "Insert part:\n" << circ << endl;
    for (int i = ori_circuit_line-1; i >= 0; i--)
    {
        if (enc_pos[i] == -1)
            continue;
        clear_circuit(tmp_circ);
        tmp_circ.set_lines(4*ori_circuit_line+2);
        tmp_circ.append_gate() = circ[enc_pos[i]];
        unsigned tmp_target = circ[enc_pos[i]].targets()[0];
        int tmp_int = key[0];
        append_toffoli(tmp_circ)(make_var(4*ori_circuit_line+1, tmp_int%2))(tmp_target);
        for (unsigned k = 1, END = tmp_circ.num_gates() - 1; k < ori_circuit_line; k++)
        {
            tmp_int /= 2;
            tmp_circ[END].add_control(make_var(4*ori_circuit_line+1 - k, tmp_int%2));
        }
        tmp_int = key[1];
        append_toffoli(tmp_circ)(make_var(4*ori_circuit_line+1, tmp_int%2))(tmp_target);
        for (unsigned k = 1, END = tmp_circ.num_gates() - 1; k < ori_circuit_line; k++)
        {
            tmp_int /= 2;
            tmp_circ[END].add_control(make_var(4*ori_circuit_line+1 - k, tmp_int%2));
        }
        append_cnot(tmp_circ, 3*ori_circuit_line, tmp_target);
        append_cnot(tmp_circ, 3*ori_circuit_line+1, tmp_target);
        circ.remove_gate_at(enc_pos[i]);
        tmp_circ = exorcism_merge_heuristic(tmp_circ);
        insert_circuit(circ, enc_pos[i], tmp_circ);
    }
    append_circuit(enc_circ, circ);

    delete enc_pos;
    delete is_enc;
    
    /*-----------------Finish encryption----------------*/
    cout << enc_circ.lines() << "," << enc_circ.num_gates() << ",";
    // Write file begin
    write_realization(enc_circ, argv[2]);
    //cout << "Encrypted circuit:\n" << enc_circ << endl;
    // Write file end
    
    cpuTime = clock() - t;
    //cout << "Encyption time: " << float(cpuTime)/CLOCKS_PER_SEC << endl;
    cout << float(cpuTime)/CLOCKS_PER_SEC << ",";
    t += cpuTime;
}

void Encryption_new_version(int argc, const char** argv)
{
    clock_t t, cpuTime;
    t = clock();
    /*-----------------Start encryption----------------*/
    circuit circ, enc_circ;
    string tmp_str;
    tmp_str = argv[1];
    tmp_str = tmp_str.substr(10, tmp_str.size() - 10); // benchmark/
    //tmp_str = tmp_str.substr(21, tmp_str.size() - 21); // revisiting_benchmark/
    tmp_str = tmp_str.substr(0, tmp_str.size() - 5); // *.real

    // Read file begin
    read_realization(circ, argv[1]);
    read_realization(enc_circ, argv[1]);
    //cout << "original circuit:\n" << circ << endl;
    // Read file end
    
    unsigned ori_circuit_line = circ.lines();
    cout << tmp_str << ",";
    cout << ori_circuit_line << ",";
    cout << circ.num_gates() << ",";

    while(enc_circ.num_gates())
    {
        enc_circ.remove_gate_at(0);
    }
    
    // add new line for encryption
    for (int i = 0; i < ori_circuit_line; i++) // add first key line
    {
        tmp_str = "K" + to_string(i+1) + "1";
        add_line_to_circuit(circ, tmp_str, tmp_str);
        add_line_to_circuit(enc_circ, tmp_str, tmp_str);
    }
    for (int i = 0; i < ori_circuit_line; i++) // add second key line
    {
        tmp_str = "K" + to_string(i+1) + "2";
        add_line_to_circuit(circ, tmp_str, tmp_str);
        add_line_to_circuit(enc_circ, tmp_str, tmp_str);
    }
    tmp_str = "rs1";
    add_line_to_circuit(circ, tmp_str, tmp_str, 0);
    add_line_to_circuit(enc_circ, tmp_str, tmp_str, 0);
    tmp_str = "rs2";
    add_line_to_circuit(circ, tmp_str, tmp_str, 0);
    add_line_to_circuit(enc_circ, tmp_str, tmp_str, 0);
    for (int i = 0; i < ori_circuit_line; i++) // add input pattern line
    {
        tmp_str = "pt" + to_string(i+1);
        add_line_to_circuit(circ, tmp_str, tmp_str, 0);
        add_line_to_circuit(enc_circ, tmp_str, tmp_str, 0);
    }

    // construct restore unit
    unsigned index_counter = 0;
    for (unsigned i = 0; i < ori_circuit_line; i++, index_counter++)
    {
        append_cnot(enc_circ,i, ori_circuit_line + i);
    }
    append_toffoli(enc_circ)(make_var(ori_circuit_line, 0))(3*ori_circuit_line);
    for (unsigned i = 1; i < ori_circuit_line; i++)
    {
        enc_circ[index_counter].add_control(make_var(ori_circuit_line + i, 0));
    }
    index_counter++;
    for (unsigned i = 0; i < ori_circuit_line; i++, index_counter++)
    {
        append_cnot(enc_circ, i, 2*ori_circuit_line + i);
    }
    append_toffoli(enc_circ)(make_var(2*ori_circuit_line, 0))(3*ori_circuit_line + 1);
    for (unsigned i = 1; i < ori_circuit_line; i++)
    {
        enc_circ[index_counter].add_control(make_var(2*ori_circuit_line + i, 0));
    }
    index_counter++;
    for (unsigned i = 0; i < ori_circuit_line; i++, index_counter++)
    {
        append_cnot(enc_circ, i, 3*ori_circuit_line + 2 + i);
    }
    for (unsigned i = 0; i < ori_circuit_line; i++, index_counter++)
    {
        append_cnot(enc_circ,i, ori_circuit_line + i);
    }
    for (unsigned i = 0; i < ori_circuit_line; i++, index_counter++)
    {
        append_cnot(enc_circ, i, 2*ori_circuit_line + i);
    }
    // Find encyption position
    int* enc_pos = new int [ori_circuit_line];
    bool* is_enc = new bool [ori_circuit_line]();
    for (int i = 0; i < ori_circuit_line; i++)
    {
        enc_pos[i] = -1;
    }
    srand(2);
    for (unsigned i = 0; i < circ.num_gates(); i++)
    {
        if (!is_enc[circ[i].targets()[0]])
        {
            is_enc[circ[i].targets()[0]] = true;
            enc_pos[circ[i].targets()[0]] = i;
        }
        else
        {
            if (rand()%2 == 0)
            {
                enc_pos[circ[i].targets()[0]] = i;
            }
        }
    }
    sort(enc_pos, enc_pos+ori_circuit_line);

    int key[2];
    key[0] = rand() % (int(pow(2, ori_circuit_line)) - 2) + 1;
    do
    {
        key[1] = rand() % (int(pow(2, ori_circuit_line)) - 1);
    } while (key[0] == key[1] || key[1] == 0);
    //cout << "key0: " << key[0] << " key1: " << key[1] << endl;

    // choose control position
    unsigned c_pos, c_pol;
    bool bc_pol;
    int tmp_int;
    c_pos = rand() % 2 == 0 ? 2 * ori_circuit_line : 3 * ori_circuit_line;
    c_pol = rand() % ori_circuit_line + 1;

    if (c_pos == 2 * ori_circuit_line)
    {   // add control in key[0]
        tmp_int = key[0];
        c_pos -= c_pol;
    }
    else
    {   // add control in key[1]
        tmp_int = key[1];
        c_pos -= c_pol;
    }
    for (unsigned i = 0, END = c_pol; i < END; i++)
    {
        c_pol = tmp_int % 2;
        tmp_int /= 2;
    }
    bc_pol = c_pol == 1 ? true : false;
    //cout << "c_pos: " << c_pos << ", c_pol: " << c_pol << endl;
    // insert encryption circuit
    circuit tmp_circ;
    //cout << "Insert part:\n" << circ << endl;
    for (int i = ori_circuit_line-1; i >= 0; i--)
    {
        if (enc_pos[i] == -1)
            continue;
        clear_circuit(tmp_circ);
        tmp_circ.set_lines(4*ori_circuit_line+2);
        tmp_circ.append_gate() = circ[enc_pos[i]];
        unsigned tmp_target = circ[enc_pos[i]].targets()[0];
        int tmp_int = key[0];
        append_toffoli(tmp_circ)(make_var(4*ori_circuit_line+1, tmp_int%2))(tmp_target);
        for (unsigned k = 1, END = tmp_circ.num_gates() - 1; k < ori_circuit_line; k++)
        {
            tmp_int /= 2;
            tmp_circ[END].add_control(make_var(4*ori_circuit_line+1 - k, tmp_int%2));
        }
        tmp_int = key[1];
        append_toffoli(tmp_circ)(make_var(4*ori_circuit_line+1, tmp_int%2))(tmp_target);
        for (unsigned k = 1, END = tmp_circ.num_gates() - 1; k < ori_circuit_line; k++)
        {
            tmp_int /= 2;
            tmp_circ[END].add_control(make_var(4*ori_circuit_line+1 - k, tmp_int%2));
        }
        append_cnot(tmp_circ, 3*ori_circuit_line, tmp_target);
        append_cnot(tmp_circ, 3*ori_circuit_line+1, tmp_target);
        // Integrate Key Input Control into Gate : 4 types rand() % 4
        switch (rand() % 18)
        {
        case 0 ... 5:
            tmp_circ[0].add_control(make_var(c_pos, bc_pol));
            tmp_circ[1].add_control(make_var(c_pos, bc_pol));
            break;
        case 6 ... 8:
            tmp_circ[0].add_control(make_var(c_pos, bc_pol));
            tmp_circ.insert_gate(1) = tmp_circ[1];
            tmp_circ[1].add_control(make_var(c_pos, !bc_pol));
            break;
        case 9 ... 11:
            tmp_circ[1].add_control(make_var(c_pos, bc_pol));
            tmp_circ.insert_gate(1) = tmp_circ[0];
            tmp_circ[1].add_control(make_var(c_pos, !bc_pol));
            break;
        case 12 ... 17:
            tmp_circ.insert_gate(1) = tmp_circ[1];
            tmp_circ.insert_gate(1) = tmp_circ[0];
            tmp_circ[1].add_control(make_var(c_pos, !bc_pol));
            tmp_circ[2].add_control(make_var(c_pos, !bc_pol));
            break;

        default:
            break;
        }

        circ.remove_gate_at(enc_pos[i]);
        tmp_circ = exorcism_merge_heuristic(tmp_circ);
        insert_circuit(circ, enc_pos[i], tmp_circ);
    }
    append_circuit(enc_circ, circ);
    
    delete enc_pos;
    delete is_enc;
    /*-----------------Finish encryption----------------*/
    cout << enc_circ.lines() << "," << enc_circ.num_gates() << ",";
    // Write file begin
    write_realization(enc_circ, argv[2]);
    //cout << "Encrypted circuit:\n" << enc_circ << endl;
    // Write file end
    
    cpuTime = clock() - t;
    //cout << "Encyption time: " << float(cpuTime)/CLOCKS_PER_SEC << endl;
    cout << float(cpuTime)/CLOCKS_PER_SEC << ",";
    t += cpuTime;
}

void Decryption(int argc, const char** argv)
{
    clock_t t, cpuTime;
    t = clock();
    /*----------------Start Find key0 and key1----------------*/
    circuit circ1;
    // Read file begin
    read_realization(circ1, argv[2]);
    //cout << "Encrypted circuit:\n" << circ1 << endl;
    // Read file end
    vector<vector<int>> expr;
    vector<vector<int>> ori_expr;
    vector<vector<int>> key_expr;
    unsigned enc_circuit_line = circ1.lines();
    unsigned critical_line = enc_circuit_line / 4;
    unsigned num_gate = circ1.num_gates();
    
    unsigned *pi = new unsigned[critical_line];
    unsigned *key0 = new unsigned[critical_line];
    unsigned *key1 = new unsigned[critical_line];
    unsigned *rs = new unsigned[2];
    unsigned *pt = new unsigned[critical_line];
    vector<Range> obfuscation_range;
    
    //cout << enc_circuit_line << endl;

    int counter = 0;
    // Identify Pi, Key, RS, pt
    for (circuit::const_iterator it = circ1.begin(); it != circ1.end(); ++it)
    {
        gate g = *it;
        // first key
        for (int i = 0; i < critical_line; i++)
        {
            if (g.controls().size() == 1)
            {
                pi[i] = g.controls()[0].line();
                key0[i] = g.targets()[0];
                g = *(++it);
                counter++;
            }
        }
        if (g.controls().size() == critical_line)
        {
            rs[0] = g.targets()[0];
            g = *(++it);
            counter++;
        }
        // second key
        for (int i = 0; i < critical_line; i++)
        {
            if (g.controls().size() == 1)
            {
                if (pi[i] != g.controls()[0].line())
                {
                    cout << "Finding key error!\n";
                    exit(1);
                }
                key1[i] = g.targets()[0];
                g = *(++it);
                counter++;
            }
        }
        if (g.controls().size() == critical_line)
        {
            rs[1] = g.targets()[0];
            g = *(++it);
            counter++;
        }
        // pt pos
        for (int i = 0; i < critical_line; i++)
        {
            if (g.controls().size() == 1)
            {
                if (pi[i] != g.controls()[0].line())
                {
                    cout << "Finding key error!\n";
                    exit(1);
                }
                pt[i] = g.targets()[0];
                g = *(++it);
                counter++;
            }
        }
        while (!isXaSet(pi, g.targets()[0], critical_line))
        {
            g = *(++it);
            counter++;
        }
        break;
    }
    
    // cout << "PI KEY0 KEY1 PT\n";
    // for (int i = 0; i < critical_line; i++)
    // {
    //     cout << pi[i] << " " << key0[i] << " " << key1[i] << " " << pt[i] << endl;
    // }
    // cout << rs[0] << " " << rs[1] << endl;
    
    // Identify Obfuscated range
    int range_counter = circ1.num_gates() - 1;
    for (circuit::const_reverse_iterator it = circ1.rbegin(); it != circ1.rend()-counter; ++it, range_counter--)
    {
        gate g = *it;
        
        if (isPi4Target(pi, g.targets()[0], critical_line) && !isPi4Control(pi, g.controls(), critical_line))
        {
            // cout << "target: " << g.targets()[0] << endl;
            Range tmp;
            tmp.target = g.targets()[0];
            tmp.range[1] = range_counter;
            // select obfuscation range
            do
            {
                g = *(++it);
                if (g.targets()[0] != tmp.target)
                {
                    --it;
                    break;
                }
                tmp.range[0] = --range_counter;
            } while(true);
            obfuscation_range.push_back(tmp);
        }
    }
    unsigned middle_generate = 0;
    range_counter = 0;
    // Use Boolean Expression to exchange original circuit
    for (const auto& ob: obfuscation_range)
    {
        //cout << "target: " << ob.target << " range: " << ob.range[0] << " - " << ob.range[1] << endl;
        // Store function with Boolean expression
        circuit::const_iterator END = circ1.begin()+ob.range[1]+1;
        for (circuit::const_iterator it = circ1.begin()+ob.range[0]; it != END; ++it)
        {
            gate g = *it;
            vector<int> tmp;
            if (g.controls().size() == 0)
            {
                tmp.push_back(2*g.targets()[0]);
            }
            else
            {
                for (const auto& c: g.controls())
                {
                    c.polarity()? tmp.push_back(2*c.line()): tmp.push_back(2*c.line()+1);
                }
            }
            expr.push_back(tmp);
            tmp.clear();
        }
        // Add restore signal in Boolean expression to eliminate restore signal
        vector<int> tmp_rs;
        tmp_rs.push_back(2*rs[0]);
        expr.push_back(tmp_rs);
        tmp_rs[0] = 2*rs[1];
        expr.push_back(tmp_rs);
        tmp_rs.clear();

        // Find out original expression and key expression
        for (const auto& cube: expr)
        {
            vector<int> tmp_ori;
            vector<int> tmp_key;
            // F_ori = F(Xa, Xb=0, Xc)
            for (const auto& literal: cube)
            {
                if (isXbSet(rs, pt, literal/2, critical_line))
                {
                    if (literal % 2 == 0) // positive control
                    {
                        tmp_ori.clear();
                        break;
                    }
                    else // negative control
                    {
                        tmp_ori.push_back(-1);
                    }
                }
                else
                {
                    tmp_ori.push_back(literal);
                }
            }
            if (tmp_ori.size() != 0)
            {
                if (tmp_ori.size() == 1 && tmp_ori[0] == -1)
                {
                    tmp_ori[0] = 2 * ob.target;
                }
                else
                {
                    for (unsigned i = 0; i < tmp_ori.size(); i++)
                    {
                        if (tmp_ori[i] == -1 && tmp_ori.size() > 1)
                        {
                            tmp_ori.erase(tmp_ori.begin() + i);
                            i--;
                        }
                        else if (tmp_ori[i] == -1)
                        {
                            tmp_ori[0] = 2 * ob.target;
                        }
                    }
                }
                ori_expr.push_back(tmp_ori);
            }
            // F_key = {F(Xa=0, Xb, Xc)} xor F(Xa=0, Xb=0, Xc)
            for (const auto& literal: cube)
            {
                if (isXaSet(pi, literal/2, critical_line))
                {
                    if (literal % 2 == 0) // positive control
                    {
                        tmp_key.clear();
                        break;
                    }
                    else // negative control
                    {
                        tmp_key.push_back(-1);
                    }
                }
                else
                {
                    tmp_key.push_back(literal);
                }
            }
            if (tmp_key.size() != 0)
            {
                if (tmp_key.size() == 1 && tmp_key[0] == -1)
                {
                    tmp_key[0] = 2 * ob.target;
                }
                else
                {
                    for (unsigned i = 0; i < tmp_key.size(); i++)
                    {
                        if (tmp_key[i] == -1 && tmp_key.size() > 1)
                        {
                            tmp_key.erase(tmp_key.begin() + i);
                            i--;
                        }
                        else if (tmp_key[i] == -1)
                        {
                            tmp_key[0] = 2 * ob.target;
                        }
                    }
                }
                key_expr.push_back(tmp_key);
            }
        }
        for (const auto& cube: expr)
        {
            vector<int> tmp_key;
            // F_key = F(Xa=0, Xb, Xc) xor {F(Xa=0, Xb=0, Xc)}
            for (const auto& literal: cube)
            {
                if (isXaSet(pi, literal/2, critical_line) || isXbSet(rs, pt, literal/2, critical_line))
                {
                    if (literal % 2 == 0) // positive control
                    {
                        tmp_key.clear();
                        break;
                    }
                    else // negative control
                    {
                        tmp_key.push_back(-1);
                    }
                }
                else
                {
                    tmp_key.push_back(literal);
                }
            }
            if (tmp_key.size() != 0)
            {
                if (tmp_key.size() == 1 && tmp_key[0] == -1)
                {
                    tmp_key[0] = 2 * ob.target;
                }
                else
                {
                    for (unsigned i = 0; i < tmp_key.size(); i++)
                    {
                        if (tmp_key[i] == -1 && tmp_key.size() > 1)
                        {
                            tmp_key.erase(tmp_key.begin() + i);
                            i--;
                        }
                        else if (tmp_key[i] == -1)
                        {
                            tmp_key[0] = 2 * ob.target;
                        }
                    }
                }
                key_expr.push_back(tmp_key);
            }
        }

        // remove original expression double cnot gate
        for (unsigned i = 0; i < ori_expr.size(); i++)
        {
            if (ori_expr[i].size() == 1 && ori_expr[i][0]/2 == ob.target)
            {
                for (unsigned k = i+1; k < ori_expr.size(); k++)
                {
                    if (ori_expr[k].size() == 1 && ori_expr[k][0]/2 == ob.target)
                    {
                        ori_expr.erase(ori_expr.begin() + k);
                        ori_expr.erase(ori_expr.begin() + i);
                        i--;
                        break;
                    }
                }
            }
        }

        // remove key expression double cnot gate
        for (unsigned i = 0; i < key_expr.size(); i++)
        {
            if (key_expr[i].size() == 1 && key_expr[i][0]/2 == ob.target)
            {
                for (unsigned k = i+1; k < key_expr.size(); k++)
                {
                    if (key_expr[k].size() == 1 && key_expr[k][0]/2 == ob.target)
                    {
                        key_expr.erase(key_expr.begin() + k);
                        key_expr.erase(key_expr.begin() + i);
                        i--;
                        break;
                    }
                }
            }
        }

        circuit key_circ;
        key_circ.set_lines(enc_circuit_line);

        // remove original and key expression double cnot gate (if key = 0 will need this)
        // if (ori_expr.size() > 1)
        // {
        //     for (unsigned i = 0; i < ori_expr.size(); i++)
        //     {
        //         if (ori_expr[i].size() == 1 && ori_expr[i][0]/2 == ob.target)
        //         {
        //             for (unsigned k = 0; k < key_expr.size(); k++)
        //             {
        //                 if (key_expr[k].size() == 1 && key_expr[k][0]/2 == ob.target)
        //                 {
        //                     ori_expr.erase(ori_expr.begin() + i);
        //                     key_expr.erase(key_expr.begin() + k);
        //                     break;
        //                 }
        //             }
        //             break;
        //         }
        //     }
        // }

        // Obtain original circuit (Replace obfuscation gates to original gates)
        //cout << "range: " << ob.range[0] << " - " << ob.range[1] << " Total: " << ob.range[1] - ob.range[0] + 1 << endl;
        for (unsigned i = 0, END = ob.range[1] - ob.range[0] + 1; i < END; i++)
        {
            circ1.remove_gate_at(ob.range[0]);
        }
        for (unsigned i = 0, END = ori_expr.size(); i < END; i++)
        {
            if (ori_expr[i][0]/2 == ob.target)
            {
                insert_not(circ1, ob.range[0] + i, ori_expr[i][0]/2);
            }
            else
            {
                insert_toffoli(circ1, ob.range[0] + i)(make_var(ori_expr[i][0]/2, (ori_expr[i][0]+1)%2))(ob.target);
                for (unsigned k = 1, ENDk = ori_expr[i].size(); k < ENDk; k++)
                {
                    circ1[ob.range[0] + i].add_control(make_var(ori_expr[i][k]/2, ((ori_expr[i][k]+1)%2)));
                }
            }
            
        }
        expr.clear();
        ori_expr.clear();
        key_expr.clear();
    }
    for (unsigned i = 0, END = counter; i < END; i++)
    {
        circ1.remove_gate_at(0);
    }
    
    //cout << "RE version:\n" << circ1 << endl;
    write_realization(circ1, "RE_version.real");
    circuit ori_circ;
    read_realization(ori_circ, argv[1]);
    //cout << "Original version:\n" << ori_circ << endl;

    delete[] pi;
    delete[] key0;
    delete[] key1;
    delete[] rs;
    delete[] pt;
    cpuTime = clock() - t;
    //cout << "Decyption time: " << float(cpuTime)/CLOCKS_PER_SEC << endl;
    cout << float(cpuTime)/CLOCKS_PER_SEC << ",";
    t += cpuTime;

    double ece_time;
    write_miter_cnf(circ1, ori_circ);
    ece_time = check_result();
    cpuTime = clock() - t;
    //cout << "ECE time: " << (float(cpuTime)/CLOCKS_PER_SEC) + ece_time << endl;
    cout << (float(cpuTime)/CLOCKS_PER_SEC) + ece_time << "\n";
    t += cpuTime;
    /*-----------------End Find key0 and key1-----------------*/
}