#!/bin/perl

# plot newest .cnv data file

my $sbe_script = 'plot_last.txt';
my $plot_program = 'C:\\Program Files (x86)\\Sea-Bird\\SBEDataProcessing-Win32\\SeaPlotW';
my $psa_file = "C:\\data\\fastcat\\current\\psa\\SeaPlot_fastcat.psa";
my($cwd) = &get_cwd();
my $run_ok = 1;

my $file_spec = "*.cnv";
my @cnv_list = `dir/b/o:-d $file_spec`;
if ( $#cnv_list < 0 )
{
  printf "-- no \*.cnv files found --\n";
  exit 1;
} else {
  printf "-- %d \*.cnv files found --\n",$#cnv_list+1;
}

chomp @cnv_list;
my $last_cnv = $cnv_list[0];

my($cnv_dir,$cnv_type,$cnv_name) = &get_file_name($last_cnv);
my $plot_cmd1= sprintf("sbebatch %s\\%s %s",$cwd,$sbe_script,$cnv_name);
my $plot_cmd2 = sprintf("\"%s\" /p%s /i%s\\%s",$plot_program,$psa_file,$cwd,$last_cnv);
printf "%s\n",$plot_cmd1;

my $stat;
if ( $run_ok )
{ 
  $stat = system ( $plot_cmd2 );
}

exit 1;

sub get_cwd
{
  my $cwd;

  $cwd = `cd`;
  chomp $cwd;

  ($cwd);
}

sub count_bottles
{
  my ($bl_file) = @_;
  my ($count);

  open(BL,$bl_file) || die "Cannot open $bl_file in read mode: $!";

  $count = 0;

  while ( ($line = <BL>) )
  {
    $count++;
  }

  close BL;

  $count -= 2;
  if ( $count < 0 ) { $count = 0 }

  ($count);
}

sub get_file_name
{
  my ($full_name) = @_;
  my (@parts,@fparts,@chars);
  my ($cbuf , $dir_name,$file_name,$file_type,@type_parts);
  my ($start);

  chomp $full_name;

  @chars = split(//,$full_name);
  if ( $chars[0] eq '.' ) 
  { 
    @chars = @chars[1..$#chars];
    $start = '.';
  } else {
    $start = "";
  }

  $full_name = join("",@chars);

  @parts = split(/\\/,$full_name);

  if ( $#parts>0 )
  {
    $dir_name = join("\\",@parts[0..$#parts-1]);
    $dir_name = sprintf("%s%s",$start,$dir_name);
  } else {
    $dir_name = '.\\';
  }
  if ( $dir_name eq '.\\' ) { $dir_name = '' }

  $cbuf = $parts[$#parts];
  ($file_name,@type_parts) = split(/\./,$cbuf);
  if ( $#type_parts > 0 )
  {
    $file_type = join("\.",@type_parts);
  } elsif ( $#type_parts == 0 ) {
    $file_type = $type_parts[0];
  } else {
    $file_type = '';
  }

  ($file_name,$file_type,$dir_name);
  ($dir_name,$file_type,$file_name);

}
