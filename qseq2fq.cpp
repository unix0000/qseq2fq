#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <getopt.h>

static struct option long_options[] = {
  {"help", no_argument, 0, 'h'},
  {"dont-filter",  required_argument, 0, 'd'},
  {0, 0, 0, 0}
};


void usage(void) {
  std::cerr << "usage: qseq2fq [-d] in_qseq.cpp > out.fq" << std::endl
	    << "options:" << std::endl
	    << "    -d, --dont-filter    don't filer reads by Illumina's purity filter" << std::endl;
  exit(1);
}


std::string sanger_qual(std::string &qual) {
  // Convert to Sanger quality
  std::string squal;
  for (unsigned i=0; i < qual.size(); i++) {
    // a bit explicit: first to Phred, then to Sanger
    squal += (char)(((int) qual[i] - 64) + 33);
  }
  return squal;
}

std::string seq_remove_dots(std::string &seq) {
  // Replace '.' with N
  std::string fixed_seq;
  for (unsigned i=0; i < seq.size(); i++) {
    if (seq[i] == '.')
      fixed_seq += 'N';
    else
      fixed_seq += seq[i];
  }
  return fixed_seq;
}


int main(int argc, char *argv[]) {
  int dont_filter = 0;
  if (argc ==1 )
    usage();

  while (true) {
    int c;
    int option_index = 0;

    c = getopt_long (argc, argv, "hd", long_options, &option_index);

    if (c == -1)
      break;
    
    switch (c) {
    case 'd':
      dont_filter = 1;
      break;
    case 'h':
      usage();
      break;
    default:
      usage();
    }
  }
      
  std::istream *qseq_file;
  if (strcmp(argv[optind], "-") == 0) {
    std::cerr << "using standard in" << std::endl;;
    qseq_file = &std::cin;
  } else {
    qseq_file = new std::ifstream(argv[optind], std::ios::in);
    if (qseq_file->fail()) {
      std::cerr << "[qseq2fq] error: cannot open file '" << argv[1] << "'." << std::endl;
      return 1;
    }
  }
  
  
  std::string line;
  int total = 0, failed_filter = 0;
  while (std::getline(*qseq_file, line)) {
    std::istringstream linestream(line);
    std::string name, seq, qual;
    int run_num, lane_num, tile_num, x, y, index, read_num, filter;
    linestream >> name >> run_num >> lane_num >> tile_num >> x >> y >> index >> read_num >> seq >> qual >> filter;
    std::string squal = sanger_qual(qual);
    std::string fixed_seq = seq_remove_dots(seq);
    if ((filter && !dont_filter) || dont_filter) {
      printf("@%s:%d:%d:%d:%d#%d/%d\n%s\n+\n%s\n", name.c_str(), lane_num, tile_num, x, y, index, read_num, fixed_seq.c_str(), sanger_qual(qual).c_str());
    } else {
      failed_filter++;
    }
    total++;
  }
  
  std::cerr << "[qseq2fq] " << failed_filter << " failed Illumina's purity filter, " << total << " total (removing failed = " << !dont_filter << ")" << std::endl;
    
  return 0;
}
