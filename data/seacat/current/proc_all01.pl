#!/bin/perl

# process all data files in list

$file_spec = "b*.hex";
@proc_list = `dir/b $file_spec`;

$count = 0;
for ( $ii=0 ; $ii<=$#proc_list ; $ii++ )
{
  ($fname,$ftype) = split(/\./,$proc_list[$ii]);
  $line = sprintf("catprocx %s",$fname);
  printf "%s\n",$line;
  system $line;
  $count++;
}

printf "\n%d files processed\n",$count;

exit 1;
