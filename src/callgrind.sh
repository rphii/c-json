#!/bin/bash
valgrind --tool=callgrind --dump-before=KonqMainWindow::slotReload ./json

