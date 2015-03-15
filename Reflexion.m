#import <CRuby.h>
#import "Reflexion.h"


void Init_rays_native ();
void Init_reflex_native ();


@implementation Reflexion

+ (void)initialize
{
	[CRuby addExtension:@"rays/native"   init:^{Init_rays_native();}];
	[CRuby addExtension:@"reflex/native" init:^{Init_reflex_native();}];

	[CRuby addLibrary:@"Reflexion" bundle:[NSBundle bundleForClass:Reflexion.class]];
}

+ (void)start
{
	[CRuby load:@"main.rb"];
}

@end
