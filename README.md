## Introduction

Proposing a practical, easy and clean way of composing perfetto categories
declared at multiple places/files across multiple independent third-party
libraries.

A TU (translation unit) of main-app or a third-party library
could be including headers of multiple third-party libraries, and hence
they could end up including the multiple perfetto-category declaration
files. With this proposed model, all of it works SEAMLESSLY requiring NO
restriction at all on how headers could be included.. Things will continue to
work fine and nothing will ever break after including a new header in some file.

These third-party libraries will also continue to have their independent life.

This new method also proposes to:

- Get rid of multiple TrackEventDataSource (controlled via namespace).
  Current methods creates a lot of problems and we are introducing more and more
  hacks to overcome those problems. We still have in-flight hacks to overcome
  the problems arising at the moment.

- Get rid of mapping multiple TrackEventDataSource to a common TLS and a
  common trace-writer. These things introduce a lot of complexity, specially
  now when we have startup-tracing, we will need to introduce that complexity
  in startup tracing as well.


## The Fundamental Problem



