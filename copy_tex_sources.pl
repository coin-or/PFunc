# This program is used to copy tex and related files from the source directory
# to the binary directory. The arguments are:
# 1. source directory
# 2. target directory
# 3. varargs -> The names of all the subdirectories to be copied.
#
# This program copies files with the following extensions:
# .tex, .cls, .sty, .eps, .bib
#

# Enforce variable declaration
use strict;

# Copying a file
use File::Copy;

# Capture the number of command line arguments
my $num_args = $#ARGV + 1;

if (2 > $num_args) {
  print "Please run this file as follows:\n \
  copy_tex_sources.pl <src> <tar> [subdir1 [subdir2] ...]\n"
} else {
  my $source_dir = @ARGV[0];
  my $destination_dir = @ARGV[1];
  my @source_dirs = ($source_dir);
  my @destination_dirs = ($destination_dir);
  my $num_subdirs = $num_args - 2;
  my $num_dirs = $num_args - 1;

  # Copy the names of all the subdirectories
  for (my $i = 0; $i < $num_subdirs; $i = $i+1) { 
    $source_dirs[$i+1] = $source_dir . "/" . @ARGV[$i+2];
    $destination_dirs[$i+1] = $destination_dir . "/" . @ARGV[$i+2];
  }

  # Copy from source to destination.
  for (my $i = 0; $i < $num_dirs; $i = $i+1) {
    # Create the destination directory if it does not exist.
    if (not -d $destination_dirs[$i]) {
      print "$destination_dirs[$i] does not exist -- creating it\n";
      mkdir ($destination_dirs[$i], 0755);
    }

    # Open the directory and read all the files.
    opendir (DIR, $source_dirs[$i]);
    my @source_files = readdir (DIR);
    closedir (DIR);

    # Copy the files with the right extension
    my $num_source_files = $#source_files + 1;
    for (my $j = 0; $j < $num_source_files; $j = $j+1) {
      my $source_file = $source_dirs[$i] . "/" . $source_files[$j];
      my $destination_file = $destination_dirs[$i] . "/" . $source_files[$j];
      if ($source_file =~ /\.tex$|\.cls$|\.sty$|\.bib$|\.eps$/) {
        print $source_file, " --> ", $destination_file, "\n"; 
        copy ($source_file, $destination_file);
      } else {
        print "Skipping: $source_file \n";
      }
    }
  }
}
