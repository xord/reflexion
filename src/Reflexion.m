#import <CRuby.h>
#import "Reflexion.h"


void Init_beeps_native ();
void Init_rays_native ();
void Init_reflex_native ();


@implementation Reflexion

+ (void)initialize
{
	static BOOL done = NO;
	if (done) return;
	done = YES;

	[CRuby addExtension:@"beeps/native"  init:^{Init_beeps_native();}];
	[CRuby addExtension:@"rays/native"   init:^{Init_rays_native();}];
	[CRuby addExtension:@"reflex/native" init:^{Init_reflex_native();}];

	for (NSString *mod in @[@"Xot", @"Rucy", @"Beeps", @"Rays", @"Reflex"])
		[CRuby addLibrary:mod bundle:[NSBundle bundleForClass:Reflexion.class]];
}

+ (BOOL)start
{
	return [self start:@"main.rb"];
}

+ (BOOL)start:(NSString *)filename
{
	return [CRuby start:filename];
}

@end
