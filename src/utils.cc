#include <utils.h>
using std::cout;
using std::endl;

namespace utils{
    std::string aDelimiter =",";
    std::string get_last_state(std::string aPath)
    {
        if(aPath.empty()){
            cout << "the given automata path is empty" <<endl;            
        }
        aPath=aPath.substr(0,aPath.size()-1);

        std::size_t found = aPath.find_last_of(aDelimiter);
        return aPath.substr(found+1);
    }

    int str2int(std::string s){
        return std::stoi(s);
    }

    std::string get_current_time() {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

        return buf;
    }

    std::string set_to_string(std::set<std::string> aSet){
        std::ostringstream stream;
        std::copy(aSet.begin(), aSet.end(), std::ostream_iterator<std::string>(stream, ","));
        std::string result = stream.str(); 
        return result.substr(0, result.size()-1);
    }

    void gen_ltl_files(std::string script, std::string build_dir, std::string formula){
        std::string cmd=script + " " + build_dir + " '" + formula +"'";
        std::cout <<"script: " <<cmd << std::endl;
        system(cmd.c_str());
    }

    void string_to_input_type(std::string p, std::vector<INPUT_TYPE>& results){
        
        std::vector<std::string> v = string2vector(p);

        for (auto e : v){
            if(!e.empty()){
                results.push_back(std::stoi(e));
            }
        }

    }

    void string_to_vector(std::string p, std::vector<std::string>& results){
    
        std::string delimiter = ";;";
        size_t pos = 0;
        std::string token;
        while ((pos = p.find(delimiter)) != std::string::npos) {
            token = p.substr(0, pos);
            p.erase(0, pos + delimiter.length());
            results.push_back(token);
        }	
    }


    std::vector<std::string> string2vector(std::string s){
        std::vector<std::string> results;
        boost::split(results, s, [](char c){return c == ',';});
        return results;
    }

    INPUT_TYPE* read_input(int* size, std::string input_file){
        std::cout <<"file path: " <<input_file<<std::endl;

        std::ifstream ifs(input_file, std::ios::binary); 

        if(ifs){
            ifs.seekg(0, ifs.end);
            *size=ifs.tellg();
            ifs.seekg(0,ifs.beg);

            INPUT_TYPE* input=new INPUT_TYPE[*size];
            
            ifs.read((char*)(&input[0]), *size);

            if(ifs)
                std::cout << "all bytes are read!"<<std::endl;
            else
                std::cout <<"error: " << ifs.gcount() << std::endl;
            
            ifs.close();
            return input;
        }
        else
            return nullptr;
    }

    void write_input(INPUT_TYPE* input, int size, std::string output_file){

        std::ofstream ofs(output_file, std::ios::binary);
        if(ofs){
            ofs.write((char*)(&input[0]), size);
        }
        ofs.close();
    }

    //For RERS
    void write_to_shmem_common(INPUT_TYPE arr[], uint32_t arr_size){
        int shmid = get_prefix_smem();
        if(shmid == -1){
            return;
        }
        else{
            PREFIX_SMEM* shm = bind_prefix_smem(shmid);
            if(shm == (PREFIX_SMEM*)-1){
                return;
            }
            else{
                memcpy(shm->array[0], arr, arr_size*sizeof(INPUT_TYPE));
                shm->arr_size = arr_size;
                printf("write to shared memory: %d\n", (shm->arr_size));
                detach_prefix_shmem(shm);
                return;
            }
        }
    }

    //For protocol
    void write_to_shmem_protocol(std::vector<std::string> arr, uint32_t arr_size){
        int shmid = get_prefix_smem();
        if(shmid == -1){
            std::cout << "Cannot Got Shared Memory. " << std::endl;
            return;
        }
        else{
            PREFIX_SMEM* shm = bind_prefix_smem(shmid);
            if(shm == (PREFIX_SMEM*)-1){
                return;
            }
            else{
                if(arr_size > 30){
                    throw std::runtime_error("Total length at shared memory overflow");
                }
				if(arr_size == 0){
					shm->arr_size = 0;
					printf("write to shm size: %u\n", shm->arr_size);
                    detach_prefix_shmem(shm);
					return;
				}
                //printf("write to shm length: %u; string: %s\n", arr_size, arr[0]);
                for(uint32_t i = 0; i < arr_size; i++){
                    if(arr[i].length() > 20000){
                        throw std::runtime_error("Single length at shared memory overflow");
                    }
                    else{
                        stpcpy((shm->array)[i], arr[i].c_str());
                    }
                    //printf("shm value: %s\n", (shm->array)[i]);
                }
                shm->arr_size = arr_size;
                printf("write to shm size: %u\n", shm->arr_size);
                detach_prefix_shmem(shm);
                return;
            }
        }
        
    }

}//namepsace

