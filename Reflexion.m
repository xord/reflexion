#import <CRuby.h>
#import "Reflexion.h"


void Init_rays_native ();
void Init_reflex_native ();


@implementation Reflexion

+ (void)initialize
{
	[CRuby addExtension:@"rays/native"   init:^{Init_rays_native();}];
	[CRuby addExtension:@"reflex/native" init:^{Init_reflex_native();}];

	for (NSString *mod in @[@"Xot", @"Rucy", @"Rays", @"Reflex"])
		[CRuby addLibrary:mod bundle:[NSBundle bundleForClass:Reflexion.class]];
}

+ (BOOL)start
{
	return [self start:@"main.rb"];
}

+ (BOOL)start:(NSString *)filename
{
	if (![CRuby load:filename])
		return NO;
	
	[CRuby evaluate:@"Reflexion.start"];
	return YES;
}

@end
