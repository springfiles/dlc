// $Id: authcache_example.js,v 1.2 2009/07/13 02:55:01 jonahellison Exp $


// Callback function from Ajax request
function _dlc_dlc(vars) {
  $("#block-dlc-5 .content").html(vars);
}

function authcacheExampleInit() {
  ajaxJson = {
    // The name of the function to call, both for ajax_authcache.php and
    // this file (see function above). The cookie value will change if
    // the user updates their block (used for max_age cache invalidation)
    'dlc_example' : $.cookie('dlc_example'),

    // Makes browser cache the Ajax response to help reduce server resources
    'max_age' : 1
  }
  
  // Perform independent Authcache ajax request
  Authcache.ajaxRequest(ajaxJson);
}

jQuery(function() { authcacheExampleInit(); })
