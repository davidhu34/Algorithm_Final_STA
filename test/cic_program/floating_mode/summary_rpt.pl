#!/usr/bin/perl
sub print_usage{

print <<ENDL;   
        
        Usage : summary <vcs logfile> 
               
ENDL
}
####################################
## ARGV Check
####################################
if ($#ARGV == -1) {
    print_usage();
    exit;
}
unless($#ARGV!=2){
    print_usage();
    exit;
}
$vcslogfile = $ARGV[0];
$vcslogfile =~/(case[\d])/s;
$case_name = $1;
$cnt=0;
$cnt_en = 0;
$stop = 0;
$path_now ="";
$path_next ="";
$pass_fail = 0;
$path_cnt=0;
$error_cnt=0;
$total_result=0;




#print "\nSTEP: Create file system.\n";
if (-e "../output/$case_name.justify.rpt") {
	unlink "../output/$case_name.justify.rpt";
}

system "/bin/touch ../output/$case_name.justify.rpt" || die "Error: Fail to create ../output/$case_name.justify.rpt\n";

open (REPORTFILE, "<$vcslogfile") || die "Error: Can't find the $vcslogfile.\n";

$justify_cmd = "****************"."\n";
&write_cmd;
$justify_cmd = "*Report Summary*"."\n";
&write_cmd;
$justify_cmd = "****************"."\n"; 
&write_cmd;


while (<REPORTFILE>){
        chomp($_);
        s/^\s+|\t//g;
	

        if($_=~ /Error/s){
          $total_result=1;
	}


        if($_=~ /error /s){
          $pass_fail =0;
	  $path_cnt=0;
	  $error_cnt++;
	}
       
        if($error_cnt==1){
	  $justify_cmd = "FAIL *"."\n"; 
          &write_cmd;
        }
	if($_=~ /path/s){
	  $error_cnt=0;
	  $path_cnt++;
	  if($path_cnt==2){
	
	  $pass_fail=1;
	  $path_cnt=1;
	  }
	  if($pass_fail==1){
	  $justify_cmd = "PASS "."\n"; 
          &write_cmd;
	  $pass_fail =0;
	  }
	
	  $_=~/([\d].*)/s;
	  $justify_cmd = "path: ".$1; 
          &write_cmd;
	
	
	}
	
	else {
	$pass_fail =0;
	}
	
	
        
}
close(REPORTFILE);

open (REPORTFILE, "<../output/$case_name.justify.rpt") || die "Error: Can't find the ../output/$case_name.justify.rpt.\n";
while (<REPORTFILE>){
chomp($_);
s/^\s+|\t//g;

#$justify_cmd = $_."\n"; 
#&write_cmd;
@pfcheck = split(/\s+/);
 if($pfcheck[2] eq "PASS"){
 #$justify_cmd = "now is ".$pfcheck[2]."\n"; 
 #&write_cmd;
 #print "now is ".$pfcheck[1].$pfcheck[2]."\n";
 $cnt++;
 }
 elsif($pfcheck[2] eq "FAIL"){
 $total_result=1;
 }
}
close(REPORTFILE);
if($total_result==1){
$justify_cmd = "\n"."Total Result: Justification Failed"."\n"; 
&write_cmd;
}
else{
$justify_cmd = "\n"."Total Result: Justification passed"."\n"; 
&write_cmd;
$justify_cmd = "\n"."total true path = ".$cnt."\n"; 
&write_cmd;
}



sub write_cmd{
#print "Notice: Write the cmd.\n";

open (WRITECMDFILE, ">>../output/$case_name.justify.rpt") || die "Error: Can't open ../output/$case_name.justify.rpt to write.\n";

print WRITECMDFILE $justify_cmd;

close (WRITECMDFILE);

}
