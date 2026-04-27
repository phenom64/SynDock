#! /usr/bin/env bash

$XGETTEXT `find default -name \*.js -o -name \*.qml -o -name \*.cpp` -o $podir/syndock_indicator_org.syndromatic.syndock.default.pot
$XGETTEXT `find org.syndromatic.syndock.plasma -name \*.js -o -name \*.qml -o -name \*.cpp` -o $podir/syndock_indicator_org.syndromatic.syndock.plasma.pot
$XGETTEXT `find org.syndromatic.syndock.plasmatabstyle -name \*.js -o -name \*.qml -o -name \*.cpp` -o $podir/syndock_indicator_org.syndromatic.syndock.plasmatabstyle.pot 
