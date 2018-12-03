//
//  BDTitleBarButton.m
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDTitleBarButton.h"
#import "BDToolBarButton+Private.h"
#import "BDButtonStyle.h"
#import "BDResourceManager.h"

@implementation BDTitleBarButton

@synthesize title = title_;
@synthesize color = color_;
@synthesize font = font_;

- (id)initWithType:(NSInteger)type
{
    if (self = [super initWithType:type])
    {
    }
    
    return self;
}

+ (BDTitleBarButton *)titleBarButtonWithStyle:(BDButtonStyle *)style
{
    BDTitleBarButton *button = [[BDTitleBarButton alloc] initWithType:style.buttonType];
    [button setupStyle:style];
    
    return [button autorelease];
}

- (void)dealloc
{
    [title_ release];
    [color_ release];
    [font_ release];
    
    [super dealloc];
}

- (void)setupStyle:(BDButtonStyle *)style
{
    BDResourceManager *resourceManager = [BDResourceManager sharedInstance];
    
    [self setFrame:style.buttonFrame];
    
    // set images
    if (style.imageBackNormal)
    {
        UIImage * normalImage = [resourceManager imageNamed:style.imageBackNormal];
        normalImage = [normalImage stretchableImageWithLeftCapWidth:normalImage.size.width  / 2
                                                       topCapHeight:normalImage.size.height / 2];

        [self setBackgroundImage:normalImage forState:UIControlStateNormal];
    }
        
    if (style.imageBackHighlighted)
    {
        UIImage *hImage = [resourceManager imageNamed:style.imageBackHighlighted];
        hImage = [hImage stretchableImageWithLeftCapWidth:hImage.size.width / 2
                                             topCapHeight:hImage.size.height / 2];
        [self setBackgroundImage:hImage forState:UIControlStateHighlighted];
    }
    
    if (style.titleFont)
    {
        UIFont *font = [resourceManager fontWithFontStyle:style.titleFont];
        self.font = font;
    }

    UIColor *color = [resourceManager colorWithHex:style.titleColor];
    self.color = color;

    self.title = style.titleString;
}

- (void)drawRect:(CGRect)rect
{
    [super drawRect:rect];
    
    if (title_)
    {
        [color_ set];
        [title_ drawInRect:rect withFont:font_ lineBreakMode: NSLineBreakByTruncatingTail alignment:NSTextAlignmentCenter];
    }
}

@end
