var progressPercent = 0;
var progressState = "none";

jsgitprogresscallback = function(progressmessage) {
  console.log(progressmessage);
  if (progressmessage.includes("Resolving deltas")) {
    progressState = "resolving";
    var regexp = /Resolving deltas ([0-9]+)\/([0-9]+)/g;
    var match = regexp.exec(progressmessage);
    if (match && match.length > 2) {
      var x = parseInt(match[1]);
      var y = parseInt(match[2]);
      if (y == 0) { progressPercent = 90; }
      else { progressPercent = 90 + Math.floor((x*10)/y); }
    }
  } else if (progressmessage.includes("chk ")) {
    progressState = "cloning";
    var regexp = /net  ([0-9]{1,3})/g;
    var match = regexp.exec(progressmessage);
    if (match && match.length > 1) {
      progressPercent = Math.floor((parseInt(match[1])*90)/100);
    }
  } else {
    progress = "none";
  }
  self.postMessage({ "cloneprogress": progressPercent });
}

jsgiterrorcallback = function(message) {
  self.postMessage({ "___ERROR___": message });
}

jsgitinit = cwrap('jsgitinit', null, []);
jsgitclone = cwrap('jsgitclone', null, ['string', 'string']);
jsgitinitrepo = cwrap('jsgitinitrepo', null, ['number']);
jsgitopenrepo = cwrap('jsgitopenrepo', null, []);
jsgitopenrepopath = cwrap('jsgitopenrepopath', null, ['string']);
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