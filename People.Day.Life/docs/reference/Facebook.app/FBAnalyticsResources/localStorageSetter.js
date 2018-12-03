function __FBAnalyticsSetLocalStorage() {
  var dict = %@;
  for (var key in dict) {
    window.localStorage.setItem(key, dict[key]);
  }
  return 'success';
}
