# This program takes in the path to the latex compiler and a tex file to 
# compile. It produces the required dvi file. A special feature of this 
# program is that it tests the *.log file for unresolved references and 
# repeatedly calls latex until the references are resolved

# Enforce variable declaration
use strict;

# Subroutine that checks for the presence of unresolved labels
sub check_unresolved_labels {
  my $log_file = @_[0];
  my $cross_refs_missing = 0;
 
  open (LOG_FILE, "$log_file") || print "Error, $log_file absent\n";
  while (<LOG_FILE>) {
    if (/Label\(s\) may have changed/) {
      $cross_refs_missing = 1;
    }
  }
  close (LOG_FILE);

  return $cross_refs_missing;
}

# Subroutine to compile a tex file given a latex compiler. The subroutine 
# uses check_unresolved_labels to recompile the given tex file again. The
# maximum number of attempts is 5 though. After which we leave the unresolved
# labels unresolved. Otherwise there maybe a infinite loop.
sub compile_tex_file {
  my $num_args = $#_ + 1;
  my $latex_compiler = @_[0];
  my $tex_file;
  my $log_file; 
  my $bibtex_compiler;
  my $compile_iteration = 0;
  my $max_compiles = 5;

  # Figure out if there is a bib file involved
  if (3 == $num_args) {
    $bibtex_compiler = @_[1];
    $tex_file = @_[2];
  } else {
    $bibtex_compiler = "NONE";
    $tex_file = @_[1];
  }
  
  # LOG file for the tex file
  $log_file = $tex_file . ".log";

  # Compile the tex file first
  system ("$latex_compiler $tex_file");

  # Check and compile the bib file if present
  if (3 == $num_args) {
    system ("$bibtex_compiler $tex_file");
  }

  # Now, iterate either 5 times or till all the labels are resolved
  do {
    system ("$latex_compiler $tex_file");
    $compile_iteration += 1;
  } while ($max_compiles > $compile_iteration && 
           1 == check_unresolved_labels($log_file))
}

# Capture the number of command line arguments
my $num_args = $#ARGV + 1;

if (2 > $num_args) {
  print "Please run this file as follows:\n \
  tex_to_dvi.pl <latex-compiler> <main-tex-file-without-tex>\n OR
  tex_to_dvi.pl <latex-compiler> <bibtex-compiler> <main-tex-file-without-tex>\n";
} else {
  # declare all the variables that we need.
  my $latex_compiler = @ARGV[0];
  my $bibtex_compiler;
  my $main_tex_file;

  if (2 == $num_args) {
    $main_tex_file = @ARGV[1];
    compile_tex_file ($latex_compiler, $main_tex_file);
  } else {
    $bibtex_compiler = @ARGV[1];
    $main_tex_file = @ARGV[2];
    compile_tex_file ($latex_compiler, $bibtex_compiler, $main_tex_file);
  }
}
