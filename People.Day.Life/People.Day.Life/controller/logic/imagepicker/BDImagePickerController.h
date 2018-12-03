//
//  BDImagePickerController.h
//  People.Day.Life
//
//  Created by samuel on 13-1-6.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDBaseController.h"

@class BDImagePickerController;
@class BDToolBarController;

@protocol BDImagePickerDelegate <NSObject>
- (void)imagePickerController:(BDImagePickerController *)picker didFinishPickingImage:(UIImage *)image
                          withInfo:(BOOL)result;
@end

@interface BDImagePickerController : BDBaseController
{
    NSInteger sourceType_;
    BDToolBarController *toolBarController_;
    id<BDImagePickerDelegate> delegate_;
}

@property (nonatomic, retain) id<BDImagePickerDelegate> delegate;

- (BOOL)initSource;

@end
