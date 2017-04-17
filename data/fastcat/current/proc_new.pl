#!/bin/perl

# process all data new fastcat data files(.hex)
# determine if they are new by looking for a .cnv file

# check .hex files in following order (bon*,*)
# and stop with first non empty list

my $run_ok = 1;
my $run_edithead = 1;
my $edit_routine = "c:\\data\\epic\\edithead";
my $sbe_script = 'fastcat_proc.txt';

my($cwd) = get_cwd();

my $file_spec = "bon*.hex";
my @proc_list = `dir/b $file_spec`;
if ( $#proc_list < 0 )
{
  printf "-- no bon\*.hex files found --\n";
} else {
  printf "-- %d bon\*.hex files found --\n",$#proc_list+1;
}

if ( $#proc_list < 0 )
{
  $file_spec = "*.hex";
  @proc_list = `dir/b $file_spec`;
  if ( $#proc_list < 0 )
  {
    printf "-- no \*.hex files found --\n";
  } else {
    printf "-- %d \*.hex files found --\n",$#proc_list+1;
  }
}

if ( $#proc_list >= 0 ) { chomp @proc_list }

my $count = 0;
my $count_p = 0;

# @proc_list = ($proc_list[0]);
my $just_processed;
my ($stat);
my ($fname);
for ( my $ii=0 ; $ii<=$#proc_list ; $ii++ )
{
  ($fname,my $ftype) = split (/\./,$proc_list[$ii]);
  my $processed_file = $fname . "\.CNV";
  my $hex_file = sprintf("%s\\%s",$cwd,$proc_list[$ii]);
  printf "Hex file = %s\n",$hex_file;
  my ($fdir,$ftype,$fname) = get_file_name($hex_file);
  printf "dir|name|type = %s|%s|%s\n",$fdir,$fname,$ftype;
  $just_processed = 0;
  if ( (-e $processed_file) ) {
    printf "%s %s\n","found file",$fname;
   } else {
    my $this_seacat = $fname;
    $cast_num = $fname;
    $cast_num =~ tr/A-Z/a-z/;
    $cast_num =~ s/[a-z\-_]//g;
    if ( length($cast_num) > 0 )
    {
      $sbe_cmd = sprintf("sbebatch %s\\%s %s",$cwd,$sbe_script,$this_seacat);
      printf "%s\n",$sbe_cmd;
      if ( $run_ok )
      { 
        $stat = system ( $sbe_cmd );
        $count_p++;
        printf "%s = %d\n",'stat',$stat;
        $just_processed = 1;
      }
    }
  }
  $count++;

  $old_name = sprintf("d%s.%s",$fname,"cnv");
  $new_name = sprintf("%s.%s",$fname,"dn");
  if ( -e $new_name && -e $old_name )
  {
    my $del_cmd = sprintf("del/f %s",$new_name);
    $stat = system ( $del_cmd );
  }
  if ( $just_processed )
  {
    $mv_cmd1 = sprintf("rename %s %s",$old_name,$new_name);
    $mv_stat = system $mv_cmd1;
  }

  $old_name = sprintf("u%s.%s",$fname,"cnv");
  $new_name = sprintf("%s.%s",$fname,"up");
  if ( -e $new_name && -e $old_name )
  {
    my $del_cmd = sprintf("del/f %s",$new_name);
    $stat = system ( $del_cmd );
  }
  if ( $just_processed )
  {
    $mv_cmd2 = sprintf("rename %s %s",$old_name,$new_name);
    $mv_stat = system $mv_cmd2;
  }

  $up_cnv = $new_name;

  if ( $run_edithead && $just_processed )
  {
    $cmd = sprintf("%s %s",$edit_routine,$up_cnv);
    $stat = system ( $cmd );
  }

}

printf "\n";
printf "%d total files processed\n",$count;
printf "%d new   files processed\n",$count_p;

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
