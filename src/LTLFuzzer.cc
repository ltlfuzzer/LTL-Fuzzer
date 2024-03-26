#include <ltlfuzzer.h>
#include <ostream>
#include <stdio.h>

ltlfuzz::LTLFuzzer::LTLFuzzer(path::PathsStore *path_store) {
  this->path_store = path_store;
  this->prefix_shmid = set_prefix_smem();
}

ltlfuzz::LTLFuzzer::~LTLFuzzer() {
  delete this->automata_handler;
  delete this->targets_store;
  release_prefix_shmem(this->prefix_shmid);
}

// flag: 0 for common programs; 1 for protocols
void ltlfuzz::LTLFuzzer::init(int flag) {
  // common parameters
  char *curDir = getenv("LTLFuzzer"); // home directory of LTL-Fuzzer
  if (curDir == NULL) {
    std::cout << "Please specify the LTL-Fuzzer home direcotry" << std::endl;
    return;
  }
  std::string PRJ_HOME(curDir);
  std::cout << "LTL-Fuzzer home directory:" << PRJ_HOME << std::endl;

  char *subjDir = getenv("SUBJECT");
  if (subjDir == NULL) {
    std::cout << "Please specify the subject directory under test" << std::endl;
    return;
  }
  std::string SUBJ(subjDir);
  std::cout << "Subject directory under test: " << SUBJ << std::endl;

  char *enChar = getenv("EXECName");
  if (enChar == NULL) {
    std::cout << "Please specify the subject name to execute" << std::endl;
    return;
  }
  std::string enStr(enChar);
  this->exec_name = enStr;
  std::cout << "Subject execution name: " << enStr << std::endl;

  char *ltlProp = getenv("LTL");
  if (ltlProp == NULL) {
    std::cout << "Please specify the LTL property under test" << std::endl;
    return;
  }
  std::string formula(ltlProp);
  std::cout << "LTL property under test: " << formula << std::endl;

  if (!flag) {
    // specific parameters for RERS
    std::cout << "Detecting RERS... " << std::endl;

    /* ----------- Default fuzzing parameters ------------*/
    this->total_time_budget = 600; // fuzzing time
    this->time_budget_one_target = 300;
    this->time_to_exploitation = 250; // smaller than time_budget_one_target
    this->aflgo_paras = " -S ef709ce2 -z exp -m 800 -A";

    this->aflgo_fuzz_path = PRJ_HOME + "AFLGo/afl-fuzz ";
    this->ltl_dir = SUBJ + "ltl_dir/";
    this->build_dir = SUBJ + "build_dir/";
    this->targets_file = SUBJ + "target/targets.txt";
    this->all_events_file = SUBJ + "all_event_dir/all_events.txt";
    ltlfuzz::load_ALL_EVENTS(this->all_events_file);
    this->output_folder = SUBJ + "output_folder/";
    this->targets_store = ltlfuzz::TargetsStore::instance();
    this->error_message = "a counterexample!";
    this->program_paras = "@@";
    this->input_folder = SUBJ + "input_folder/";
    this->input = utils::read_input(
        &(this->size), this->input_folder + "input"); // one initial seed
    this->events_mapping_file = SUBJ + "event_map_dir/event_mapping.txt";
    this->targets_store->load_events(this->events_mapping_file);
    this->targets_store->load_targets(this->targets_file, 0);
    this->path_store->insert_init_automata_path("0,", "", "1");
  } else {
    // specific parameters for protocols
    std::cout << "Detecting Protocols... " << std::endl;

    /* ----------- Default fuzzing parameters ------------*/
    this->total_time_budget = 86400;
    this->time_budget_one_target = 2700;
    this->time_to_exploitation = 2400; // smaller than time_budget_one_target
    this->aflgo_paras = " -S ef709ce2 -z exp -m 1024 -K";
    this->network_link = "-N tcp://172.18.0.2/23";
    this->protocol_name = "-P TELNET";

    this->aflgo_fuzz_path = PRJ_HOME + "AFLGo/afl-fuzz ";
    this->ltl_dir = SUBJ + "ltl_dir/";
    this->build_dir = SUBJ + "build_dir/";
    this->targets_file = SUBJ + "targets/targets.txt";
    this->all_events_file = SUBJ + "all_event_dir/all_events.txt";
    ltlfuzz::load_ALL_EVENTS(this->all_events_file);
    this->input_folder = SUBJ + "input_folder";
    this->output_folder = SUBJ + "output_folder/";
    this->targets_store = ltlfuzz::TargetsStore::instance();

    this->dictionary = SUBJ + "telnet.dict";
    this->prefixDir = SUBJ + "prefix";
    std::string rm_prefix = std::string("sudo rm -r ") + this->prefixDir;
    system(rm_prefix.c_str());
    std::string mk_prefix = std::string("mkdir ") + this->prefixDir;
    system(mk_prefix.c_str());
    this->targets_store->load_targets(this->targets_file, 1);
  }

  std::string exclusive_events = utils::set_to_string(ltlfuzz::ALL_EVENTS);
  std::cout << "exclusive events: " << exclusive_events << std::endl;

  lfz::automata::Automata *atm = new lfz::automata::Automata();
  atm->set_formula(formula, exclusive_events);

  this->automata_handler = new ltlfuzz::AutomataHandler(atm);

  utils::gen_ltl_files(std::string(PRJ_HOME) + "scripts/write_ltl_file.sh",
                       this->ltl_dir, formula + ":" + exclusive_events);

  setenv("DRY_RUN", "0", 1);
  setenv("LTL", "1", 1);
}

// Flag: 0 for common fuzzed programs; 1 for protocols
void ltlfuzz::LTLFuzzer::fuzz(int flag) {

  long int start = static_cast<long int>(time(NULL));

  while ((start + this->total_time_budget) >
         static_cast<long int>(time(NULL))) {

    std::string spath = "";
    std::string prefix = "";
    if (flag) {
      std::pair<std::string, std::string> ppair =
          this->path_store->select_automataPath_and_prefix(this->prefixDir);
      spath = ppair.first;
      prefix = ppair.second;
    } else {
      std::pair<ltlfuzz::Prefix, ltlfuzz::AutomataPath> pair =
          this->path_store->select_prefix_aPath();
      ltlfuzz::Prefix prefix_o = pair.first;
      ltlfuzz::AutomataPath aPath = pair.second;
      spath = aPath.path;
      prefix = prefix_o.prefix;
    }

    std::cout << "selected aPath: " << spath << " prefix: " << prefix
              << std::endl;

    std::string lastState = utils::get_last_state(spath);
    std::cout << "last state: " << lastState << std::endl;

    std::string selected_event =
        this->automata_handler->select_event(lastState, spath);
    std::cout << "selected event: " << selected_event << std::endl
              << std::flush;

    ltlfuzz::TargetLocation target =
        this->targets_store->getTarget(selected_event, flag);

    switch (target.targetType) {
    case ltlfuzz::TargetType::INPUT:

      replace_prefix_run_program(
          prefix + this->targets_store->decode(target.targetName));

      break;

    case ltlfuzz::TargetType::OUTPUT:
      if (flag) { // For protocols
        std::vector<std::string> pre_;
        if (!prefix.empty()) {
          utils::string_to_vector(prefix, pre_);
          utils::write_to_shmem_protocol(
              pre_, pre_.size()); // AFLGo will fetch the prefix
        } else {
          utils::write_to_shmem_protocol(pre_, 0); // AFLGo will fetch the
                                                   // prefix
        }
      } else {
        std::vector<INPUT_TYPE> pre_;
        if (!prefix.empty()) {
          utils::string_to_input_type(prefix, pre_);
          utils::write_to_shmem_common(
              &pre_[0], pre_.size()); // AFLGo will fetch the prefix
        } else {
          pre_.push_back(0);
          utils::write_to_shmem_common(&pre_[0], 0);
        }
      }

      std::string cmd = assemble_cmd(target.targetName, flag);
      std::cout << cmd << std::endl;
      system(cmd.c_str());

      break;
    }
    if (!flag) {
      this->path_store->dump();
    }
  }
  if (!flag) {
    this->path_store->clean_up();
  }
}

void ltlfuzz::LTLFuzzer::replace_prefix_run_program(std::string prefix) {

  if (!prefix.empty()) {
    std::vector<INPUT_TYPE> pre_;
    utils::string_to_input_type(prefix, pre_);
    memcpy(this->input, &pre_[0], sizeof(INPUT_TYPE) * pre_.size());
  }
  std::string input_file = this->input_folder + "input";
  utils::write_input(this->input, this->size, input_file);

  std::ifstream ifs(this->targets_file);
  std::string fline;
  getline(ifs, fline);
  ifs.close();

  size_t loc = fline.find_last_of(":");
  std::string program = fline.substr(0, loc);

  std::string workdir = this->build_dir + program;
  std::string exec =
      this->build_dir + program + "/" + this->exec_name + " " + input_file;

  std::string CMD = std::string("cd ") + workdir + " && " + exec + " 2>&1";
  FILE *output = popen(CMD.c_str(), "r");

  std::ostringstream stm;
  char line[1024];
  if (output) {
    while (!feof(output)) {
      if (fgets(line, 100, output) != NULL) {
        stm << line;
      }
    }
    pclose(output);
  }

  if (is_counterexample(stm.str())) {
    std::cout << "there is a conterexample!" << std::endl;
    save_input(input_file, this->output_folder + "crashes/");
  }
}

void ltlfuzz::LTLFuzzer::save_input(std::string input_file,
                                    std::string folder) {

  std::string mk_crashes = folder;
  system(mk_crashes.c_str());
  std::string saved_file = folder + "input-" + utils::get_current_time();
  std::string CMD = "mv " + input_file + " " + saved_file;
  system(CMD.c_str());
}

bool ltlfuzz::LTLFuzzer::is_counterexample(std::string output) {

  size_t found = output.find(this->error_message);
  if (found != std::string::npos)
    return true;
  else
    return false;
}

std::string ltlfuzz::LTLFuzzer::assemble_cmd(std::string target, int flag) {
  std::string full_CMD = "";
  if (flag) {
    /** For protocols **/
    std::string CMD =
        std::string("timeout ") + std::to_string(this->time_budget_one_target) +
        " " + this->aflgo_fuzz_path + this->aflgo_paras + " -c " +
        std::to_string(this->time_to_exploitation) + " -i " +
        this->input_folder + "  -o " + this->output_folder + "fuzzing-" +
        utils::get_current_time() + " " + this->network_link + " -x " +
        this->dictionary + " " + this->protocol_name + " " + this->build_dir +
        target + "/examples/telnet-server/" + this->exec_name;

    std::string work_dir = this->build_dir + target + "/examples/telnet-server";
    std::string cl_testDir =
        std::string("rm -r ") + work_dir + std::string("/test_dir");
    std::string mk_testDir =
        std::string("mkdir ") + work_dir + std::string("/test_dir");
    system(cl_testDir.c_str());
    system(mk_testDir.c_str());
    std::string cd_testDir =
        std::string("cd ") + work_dir + std::string("/test_dir");
    full_CMD = cd_testDir + " && " + CMD;
  } else {
    /** For RERS **/
    std::string CMD =
        std::string("timeout ") + std::to_string(this->time_budget_one_target) +
        " " + this->aflgo_fuzz_path + this->aflgo_paras + " -c " +
        std::to_string(this->time_to_exploitation) + " -i " +
        this->input_folder + "  -o " + this->output_folder + "fuzzing-" +
        utils::get_current_time() + " " + this->build_dir + target + "/" +
        this->exec_name + " " + this->program_paras;

    std::string work_dir = this->build_dir + target;
    std::string cd_workDir = std::string("cd ") + work_dir;
    full_CMD = cd_workDir + " && " + CMD;
  }
  std::cout << full_CMD << std::endl;
  return full_CMD;
}
