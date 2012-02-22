#! /usr/bin/perl

my $blue = `tput setaf 4`;
my $cyan = `tput setaf 6`;
my $green = `tput setaf 2`;
my $red = `tput setaf 1`;
my $std = `tput setaf 0`;
my $white = `tput setaf 7`;

my $FAIL = "${red}FAIL$std";
my $PASS = "${green}PASS$std";

my $fail = 0;

sub quote ($)
{
  my ($in) = @_;
  $in =~ s/(["$\\`])/\\$1/g;
  $in =~ s/^[ \t]*//g;
  $in =~ s/[ \t]*$//g;
  $in;
}

# Parse and pretty-print.
sub pp ($$)
{
  my ($prog, $in) = @_;
  $in = quote($in);
  my $res = `echo  '$in' | $prog`;
  die "failed to run: echo '$in' | $prog: $?"
    if $? && $? != 256;
  chomp($res);
  $res;
}

sub check_rat_exp
{
  my ($verbose, $prog, @file) = @_;

  foreach(@file)
    {
      print STDERR "$file\n";
      open(IN, $_);
      foreach(<IN>)
        {
          $_ =~ m/\|([^|]*)\|([^|]*)\|/;
          my $l = $1;
          my $r = $2;
          my $L = pp($prog, $l);
          my $R = pp($prog, $r);

          # Empty result is a sign of failure.
          if ($L ne '' && $L eq $R)
            {
              print "$PASS: $l == $r\n";
            }
          else
            {
              print "$FAIL: $l == $r: $L != $R\n";
              if ($verbose)
                {
                  print "$l:\n";
                  pp($prog, $l);
                  print "$r:\n";
                  pp($prog, $r);
                }
              ++$fail;
            }
        }
    }
}

my $verbose = 1;
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
