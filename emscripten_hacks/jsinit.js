var progressPercent = 0;
var progressState = "none";

jsgitprogresscallback = function(progressmessage) {
    console.log(progressmessage);
    if (progressmessage.includes("Resolving deltas")) {
      progressState = "resolving";
    } else if (progressmessage.includes("chk ")) {
      progressState = "cloning";
    } else {
      progress = "none";
    }
}

jsgitinit = cwrap('jsgitinit', null, []);
jsgitclone = cwrap('jsgitclone', null, ['string', 'string']);
jsgitinitrepo = cwrap('jsgitinitrepo', null, ['number']);
jsgitopenrepo = cwrap('jsgitopenrepo', null, []);
jsgitadd = cwrap('jsgitadd', null, ['string']);
jsgitsetuser = cwrap('jsgitsetuser', null, ['string', 'string']);
jsgitresolvemergecommit = cwrap('jsgitresolvemergecommit', null, []);
jsgitremove = cwrap('jsgitremove', null, ['string']);
jsgitworkdirnumberofdeltas = cwrap('jsgitworkdirnumberofdeltas', 'number', []);
jsgitstatus = cwrap('jsgitstatus', 'number', []);
jsgitaddfileswithchanges = cwrap('jsgitaddfileswithchanges', null, []);
jsgitpush = cwrap('jsgitpush', null, []);
jsgitpull = cwrap('jsgitpull', null, []);
jsgitshutdown = cwrap('jsgitshutdown', null, []);
jsgitprintlatestcommit = cwrap('jsgitprintlatestcommit', null, []);
jsgitcommit = cwrap('jsgitcommit', null, ['string']);
jsgithistory = cwrap('jsgithistory', null, []);
jsgitregisterfilter = cwrap('jsgitregisterfilter', null, ['string', 'string', 'number']);

jsgitstartwalk = cwrap('jsgitstartwalk', null, []);
jsgitwalknextcommit = cwrap('jsgitwalknextcommit', 'string', []);
jsgitendwalk = cwrap('jsgitendwalk', null, []);