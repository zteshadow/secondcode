// Copyright 2004-present Facebook. All Rights Reserved.

#import <Foundation/Foundation.h>
#import "RGBColor.h"
#import "UIColor+Additions.h"

// Styles
#define NAVBAR_TINT         [UIColor facebookBlueTint]
#define LANDSCAPE_MODE      YES

// Enable API profiling on the endpoint (only works on sandboxes)
#define FBDFLAG_API_PROFILING 0

// FB API
#ifdef BUILD_PROD
#define FbAppSecret @"0f140aabedfb65ac27a739ed1a2263b1"
#define FbAPIKey    @"c472d0fc3dd5ce6728fc947dbf2a8b73"
#define FbAppId     @"237759909591655"
#elif defined(BUILD_IN_HOUSE)
#define FbAppSecret @"8fcd50b78ec74d78e9ce625a1167cd80"
#define FbAPIKey    @"8c568d5faf15c15255d863739f9a3b92"
#define FbAppId     @"184182168294603"
#else
// Assume developer build
#define FbAppSecret @"efdd8e8641e3a8961c9300a7a1bfbaa7"
#define FbAPIKey    @"e2ffbefcf2b135708f8ffdf41cc6612b"
#define FbAppId     @"202805033077166"
#endif

// NSUserDefault Keys
#define NSUD_LAST_SYNC_TIME  @"LastSyncTime"

// NSNotifications
#define ORCA_APP_SYNC_PENDING_CHANGED_NOTIFICATION  @"OrcaAppSyncPendingChanged"
#define ORCA_APP_MESSAGES_PREFETCHED_NOTIFICATION   @"OrcaAppMessagesPrefetched"
#define ORCA_APP_SHOW_LOCATION_HINT                 @"OrcaAppShowLocationHint"
#define ORCA_APP_RECEIVED_MESSAGE_NOTIFICATION      @"OrcaAppReceivedMessage"
#define ORCA_APP_SENT_MESSAGE_NOTIFICATION          @"OrcaAppSentMessage"
#define ORCA_APP_RECEIVED_READ_RECEIPT_NOTIFICATION @"OrcaReceivedReadReceipt"
#define ORCA_APP_RECEIVED_WEBRTC_MESSAGE            @"OrcaAppReceivedWebRTCMessage"
#define ORCA_APP_SENT_VOIP_CALL                     @"OrcaAppSentVOIPCall"

// Notification type
#define WEBRTC_NOTIFICATION_TYPE @"webrtc_incoming"

// iPhone UI Elements
#define IPHONE_UNDER_STATUS_HEIGHT              460
#define IPHONE_UNDER_NAVBAR_HEIGHT_PORT         416
#define IPHONE_UNDER_NAVBAR_HEIGHT_LAND         268
#define IPHONE_BETWEEN_NAVBAR_AND_TABBAR_HEIGHT 367
#define IPHONE_NAVBAR_HEIGHT                    44
#define IPHONE_TABBAR_HEIGHT                    49
#define IPHONE_STATUS_HEIGHT                    20

// App name for UserAgent
extern NSString *const FBMessengerAppName;

// Methods
BOOL IsPortraitOrientation(void);
BOOL IsHighResScreen(void);
BOOL IsIPad(void);

CGSize SizeForText(NSString *text, UIFont *font, float maxWidth, float maxHeight);

BOOL IsNotEmpty(NSString* str);
BOOL IsNilOrEmpty(NSString* str);
BOOL StringsAreEqual(NSString* str1, NSString* str2);

NSString* LocationServicesSettingsLink(void);


// Common localized strings
// Please modify FacebookObjcCommonStringsLinter when adding strings here
// added as #defines so the strings are pulled from the caller's bundle
// these duplciate the definitions in FBCoreLocale.h, as those weren't designed to work with orca's
// bundle structure
// we should move those methods into some framework with it's own bundle for localizations.
#define FBMLocalizedCancelButtonTitle() FBLocalizedString(@"Cancel", @"Title for generic Cancel button.")
#define FBMLocalizedOKButtonTitle() FBLocalizedString(@"OK", @"Title for generic OK button.")
#define FBMLocalizedDoneButtonTitle() FBLocalizedString(@"Done", @"Title for generic Done button that dismisses a view. This needs to be a short string to avoid truncation.")
