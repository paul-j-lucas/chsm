# CHSM: Concurrent Hierarchical State Machine

**CHSM** is a language system
for specifying concurrent, hierarchical, finite state machines
(an implementation of
‘‘[statecharts](http://en.wikipedia.org/wiki/Statechart#Harel_statechart)’’)
to model and control reactive systems.

CHSM uses its own statechart specification langauge
annotated with either C++ or Java code fragments
in the tradition of yacc grammars with C code fragments.
The generated code is fully object oriented
allowing multiple state machines to exist concurrently.
The CHSM run-time library is small, efficient, and thread-safe.

When using CHSM with Java,
you can alternatively specify a statechart using
[Groovy](http://www.groovy-lang.org)
instead of using CHSM's specification language.

CHSM has been used successfullly in production environments
such as CERN, Philips, Qualcomm,
and for the
[ANTARES](http://antares.in2p3.fr/) neutrino telescope project.

Although the current implementation has evolved over time,
the [original description](https://www.dropbox.com/s/06xf5wcsnhi0wht/pjl-chsm-thesis.pdf)
is also available.

**Paul J. Lucas**  
San Francisco, California, USA  
17 January 2018
