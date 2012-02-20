#! /usr/bin/perl

my $blue = `tput setaf 4`;
my $cyan = `tput setaf 6`;
my $green = `tput setaf 2`;
my $red = `tput setaf 1`;
my $white = `tput setaf 7`;

my $FAIL = "${red}FAIL$white";
my $PASS = "${green}PASS$white";

my $fail = 0;

sub quote ($)
{
  my ($in) = @_;
  $in =~ s/(["$\\`])/\\$1/g;
  $in =~ s/^[ \t]*//g;
  $in =~ s/[ \t]*$//g;
  $in;
}

sub check_rat_exp
{
  my ($verbose, @file) = @_;

  foreach(@file)
    {
      open(IN, $_);
      foreach(<IN>)
        {
          $_ =~ m/\|([^|]*)\|([^|]*)\|/;
          my $l = quote $1;
          my $r = quote $2;
          my $L = `echo  '$l' | ./main`;
          chomp($L);
          my $R = `echo  '$r' | ./main`;
          chomp($R);

          if($L eq $R)
            {
              print "$PASS: $l == $r\n";
            }
          else
            {
              print "$FAIL: $l == $L != $r == $R\n";
              if ($verbose)
                {
                  print "$l:\n";
                  `echo  '$l' | ./main`;
                  print "$r:\n";
                  `echo  '$r' | ./main`;
                }
              ++$fail;
            }
        }
    }
}

my $verbose = 0;
check_rat_exp($verbose, @ARGV);

exit !!$fail;


### Setup "GNU" style for perl-mode and cperl-mode.
## Local Variables:
## perl-indent-level: 2
## perl-continued-statement-offset: 2
## perl-continued-brace-offset: 0
## perl-brace-offset: 0
## perl-brace-imaginary-offset: 0
## perl-label-offset: -2
## cperl-indent-level: 2
## cperl-brace-offset: 0
## cperl-continued-brace-offset: 0
## cperl-label-offset: -2
## cperl-extra-newline-before-brace: t
## cperl-merge-trailing-else: nil
## cperl-continued-statement-offset: 2
## End:
