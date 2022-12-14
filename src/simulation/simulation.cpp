/**
 * This file contains implementations for the methods defined in the Simulation
 * class.
 *
 * You'll probably spend a lot of your time here.
 */

#include "simulation/simulation.h"
#include "page/page.h"
#include <stdexcept>

Simulation::Simulation(FlagOptions& flags)
{
    this->flags = flags;
    this->frames.reserve(this->NUM_FRAMES);
    
}

void Simulation::run() {
  for(int i =0;i<this->NUM_FRAMES;i++){
   this->free_frames.push_back(i);
   this->frames.emplace_back();
  }
  for(int i = 0; i < this->virtual_addresses.size(); i ++){
      time = i + 1;
      perform_memory_access(virtual_addresses[i]);
    }
  print_summary();
}

char Simulation::perform_memory_access(const VirtualAddress& virtual_address) {
  this->processes[virtual_address.process_id]->memory_accesses++;
  std::cout << virtual_address << "\n";
  if(this->processes[virtual_address.process_id]->is_valid_page(virtual_address.page)){
     PageTable::Row pageRow = this->processes[virtual_address.process_id]->page_table.rows[virtual_address.page];
     if(pageRow.present){
       this->processes[virtual_address.process_id]->page_table.rows[virtual_address.page].last_accessed_at = this->time;
       std::cout << "   -> IN MEMORY\n";
     }
     else{
       std::cout << "   -> PAGE FAULT\n";
       handle_page_fault(this->processes[virtual_address.process_id], virtual_address.page);
     }
     std::cout << "   -> physical address " << PhysicalAddress(this->processes[virtual_address.process_id]->page_table.rows[virtual_address.page].frame, virtual_address.offset) << "\n";
     if(frames[this->processes[virtual_address.process_id]->page_table.rows[virtual_address.page].frame].contents->is_valid_offset(virtual_address.offset)){
       std::cout << "   -> RSS: " << processes[virtual_address.process_id]->get_rss() << "\n";
       return frames[this->processes[virtual_address.process_id]->page_table.rows[virtual_address.page].frame].contents->get_byte_at_offset(virtual_address.offset);
     }
     else{
       std::cout << "SEGFAULT - INVALID OFFSET";
       exit(1);
     }
  }
  else{
    std::cout << "SEGFAULT - INVALID PAGE\n";
    exit(1);
  }
}

void Simulation::handle_page_fault(Process* process, size_t page) {
  if(process->page_table.get_present_page_count() >= this->flags.max_frames){
    if(this->flags.strategy == ReplacementStrategy::LRU){
      frames[process->page_table.rows[process->page_table.get_least_recently_used_page()].frame].set_page(process, page);
      process->page_table.rows[page].frame = process->page_table.rows[process->page_table.get_least_recently_used_page()].frame;
      process->page_table.rows[process->page_table.get_least_recently_used_page()].present = false;
    }
    else{
      frames[process->page_table.rows[process->page_table.get_oldest_page()].frame].set_page(process, page);
      process->page_table.rows[page].frame = process->page_table.rows[process->page_table.get_oldest_page()].frame;
      process->page_table.rows[process->page_table.get_oldest_page()].present = false;
    }
  }
  else{
    frames[this->free_frames.front()].set_page(process, page);
    process->page_table.rows[page].frame = this->free_frames.front();
    this->free_frames.pop_front();
  }
  process->page_table.rows[page].loaded_at = this->time;
  process->page_table.rows[page].last_accessed_at = this->time;
  process->page_table.rows[page].present = true;
  process->page_faults += 1;
  this->page_faults++;
}

void Simulation::print_summary() {
    if (!this->flags.csv) {
        boost::format process_fmt(
            "Process %3d:  "
            "ACCESSES: %-6lu "
            "FAULTS: %-6lu "
            "FAULT RATE: %-8.2f "
            "RSS: %-6lu\n");

        for (auto entry : this->processes) {
            std::cout << process_fmt
                % entry.first
                % entry.second->memory_accesses
                % entry.second->page_faults
                % entry.second->get_fault_percent()
                % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "\n%-25s %12lu\n"
            "%-25s %12lu\n"
            "%-25s %12lu\n");

        std::cout << summary_fmt
            % "Total memory accesses:"
            % this->virtual_addresses.size()
            % "Total page faults:"
            % this->page_faults
            % "Free frames remaining:"
            % this->free_frames.size();
    }

    if (this->flags.csv) {
        boost::format process_fmt(
            "%d,"
            "%lu,"
            "%lu,"
            "%.2f,"
            "%lu\n");

        for (auto entry : processes) {
            std::cout << process_fmt
                % entry.first
                % entry.second->memory_accesses
                % entry.second->page_faults
                % entry.second->get_fault_percent()
                % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "%lu,,,,\n"
            "%lu,,,,\n"
            "%lu,,,,\n");

        std::cout << summary_fmt
            % this->virtual_addresses.size()
            % this->page_faults
            % this->free_frames.size();
    }
}

int Simulation::read_processes(std::istream& simulation_file) {
    int num_processes;
    simulation_file >> num_processes;

    for (int i = 0; i < num_processes; ++i) {
        int pid;
        std::string process_image_path;

        simulation_file >> pid >> process_image_path;

        std::ifstream proc_img_file(process_image_path);

        if (!proc_img_file) {
            std::cerr << "Unable to read file for PID " << pid << ": " << process_image_path << std::endl;
            return 1;
        }
        this->processes[pid] = Process::read_from_input(proc_img_file);
    }
    return 0;
}

int Simulation::read_addresses(std::istream& simulation_file) {
    int pid;
    std::string virtual_address;

    try {
        while (simulation_file >> pid >> virtual_address) {
            this->virtual_addresses.push_back(VirtualAddress::from_string(pid, virtual_address));
        }
    } catch (const std::exception& except) {
        std::cerr << "Error reading virtual addresses." << std::endl;
        std::cerr << except.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Error reading virtual addresses." << std::endl;
        return 1;
    }
    return 0;
}

int Simulation::read_simulation_file() {
    std::ifstream simulation_file(this->flags.filename);
    // this->simulation_file.open(this->flags.filename);

    if (!simulation_file) {
        std::cerr << "Unable to open file: " << this->flags.filename << std::endl;
        return -1;
    }
    int error = 0;
    error = this->read_processes(simulation_file);

    if (error) {
        std::cerr << "Error reading processes. Exit: " << error << std::endl;
        return error;
    }

    error = this->read_addresses(simulation_file);

    if (error) {
        std::cerr << "Error reading addresses." << std::endl;
        return error;
    }

    if (this->flags.file_verbose) {
        for (auto entry: this->processes) {
            std::cout << "Process " << entry.first << ": Size: " << entry.second->size() << std::endl;
        }

        for (auto entry : this->virtual_addresses) {
            std::cout << entry << std::endl;
        }
    }

    return 0;
}
