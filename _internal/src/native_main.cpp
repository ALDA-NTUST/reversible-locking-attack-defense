#include "legacy_experiment.cpp"
int main(int argc, const char** argv) {
 if(argc != 4) { std::cerr << "Internal backend: lock|defend|attack|verify input reference-or-output\n"; return 2; }
 std::string mode(argv[1]);
 const char* args[] = {argv[0], argv[2], argv[3]};
 try {
 if(mode=="lock") Encryption_original_version(3,args);
 else if(mode=="defend") Encryption_new_version(3,args);
 else if(mode=="attack") { const char* a[]={argv[0],argv[3],argv[2]}; Decryption(3,a); }
 else if(mode=="verify") { circuit a,b; if(!read_realization(a,argv[2]) || !read_realization(b,argv[3])) return 2; write_miter_cnf(a,b); check_result(); }
 else return 2;
 } catch(const std::exception& e) { std::cerr << e.what() << "\n"; return 2; }
 return 0;
}
