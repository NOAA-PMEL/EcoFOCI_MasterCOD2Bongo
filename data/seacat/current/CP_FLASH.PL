#!/bin/perl

# copy files from c:\data\seacat\fa0501 to flash at e:\fa0501\fa0501_seacat

if ( $#ARGV >= 0 )
{
  $cp_flag = $ARGV[0];
  $cp_flag =~ s/\s+//g;
  $cp_flag =~ tr/A-Z/a-z/;
}

$last_flag = ( $#ARGV > 0 ) ? 1 : 0;
$last_flag = 1;

$e_dir = "e:\\fa0501\\fa0501_seacat\\";
@e_files = `dir/b/o:d $e_dir\*.*`;
chomp @e_files;

for ( $i=0 ; $i<=$#e_files ; $i++ )
{
  printf "%3d %s%s\n",$i+1,$e_dir,$e_files[$i];
}
printf "\n";

$c_dir = "c:\\data\\seacat\\fa0501\\";
printf "%s %s\n",'directory',$c_dir;
@c_files = `dir/b/o:d ${c_dir}\\*.*`;
chomp @c_files;

for ( $i=0 ; $i<=$#c_files ; $i++ )
{
  printf "%3d %s%s\n",$i+1,$c_dir,$c_files[$i];
}
printf "\n";

$last = ( $last_flag ) ? $#c_files : $#c_files - 1;
for ( $i=0 ; $i<=$last ; $i++ )
{
  if ( &have_file($c_files[$i],@e_files[$i]) == 0 )
  {
    $cp_file = $c_files[$i];
    &copy1($cp_file,$c_dir,$e_dir,$cp_flag);
    ## $cp_cmd = sprintf("copy %s%s %s",$e_dir,$e_files[$i],$c_dir);
    ## printf "%s\n",$cp_cmd;
  }
}

exit 1;

sub have_file
{
  my ($find,@check_list) = @_;
  my ($i , $cbuf1 , $cbuf2);
  my ($found);

  $found = $i = 0;

  $cbuf1 = $find;
  $cbuf1 =~ tr/A-Z/a-z/;
  while ( ! $found && $i<=$#check_list )
  {
    $cbuf2 = $check_list[$i++];
    $cbuf2 =~ tr/A-Z/a-z/;
    if ( $cbuf1 eq $cbuf2 ) { $found = 1 }
  }

  ($found);

}

sub copy1
{
  my ($from_file,$from_dir,$to_dir,$flag) = @_;
  my (@types);
  my ($name,$type,$i);
  my ($cp_from , $cp_file);
  my ($stat);
  my ($cp_ok);

  @types = qw(dat bl con cnv);

  $cp_ok = ( $cp_flag eq 'cp' ) ? 1 : 0;

  ($name,$type) = split(/\./,$from_file);

  $cp_from = sprintf("%s%s",$from_dir,$from_file);
  if ( -e $cp_from )
  {
    $cp_cmd = sprintf("copy %s %s",$cp_from,$to_dir);
    printf "%s\n",$cp_cmd;
    if ( $cp_ok ) { $stat = system ( $cp_cmd ) }
  } else {
    printf "%s NOT FOUND\n",$cp_from;
  }

}
