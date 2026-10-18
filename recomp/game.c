/* Ego: Repton 4 (Superior Software, 1992): what libroger needs to know
 * about the game.
 *
 * !Run: WimpSlot 640K, Set Repton4$Dir to the application, run !RunImage:
 * a BASIC program of four lines - MODE 13, OFF, *ScreenLoad LoadScreen
 * (the title picture) and *Run ObjCode. Those are done here (init), and
 * ObjCode is the program: an absolute at &8000, entered at &1E9B8.
 *
 * There, before the game, is its copy protection: an ADFS_DiscOp verify of
 * the disc's last track (disc address &C6C00), which has to fail - the
 * original's last track is unformatted, and a dump of it stops before
 * that track - and then OS_File 13 on
 * adfs::Repton4.$.!Repton4.Resources.Panel (the disc by its name), which
 * has to be there. Either check failing hangs the program. Both run here,
 * and pass, as they did on the original: the disc in the drive is the
 * image (../extracted/disc1.img, recipe.json's "raw"), whose last track
 * libroger has unformatted because the dump stops short of it. */
#include <roger.h>

extern const struct roger_image recomp_image;      /* gen/entries.c */

/* <Repton4$Dir>.x is !Repton4.x; so is the disc by name,
 * adfs::Repton4.$.!Repton4.x */
static const char *const prefixes[] = {"adfs::Repton4.$.", "ADFS::Repton4.$.", ":Repton4.$.", NULL};
static const char *const variables[] = {"Repton4$Dir", "!Repton4", NULL};

static void sys(uint32_t n, uint32_t r0, uint32_t r1, uint32_t r2)
{
    R[0] = r0; R[1] = r1; R[2] = r2;
    roger_swi(n);
}

/* !RunImage, up to its *Run ObjCode: MODE 13 (the initial mode), OFF
 * (VDU 23,1,0;0;0;0;) and the title picture */
static void init(void)
{
    static const uint8_t off[] = {23, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    for (unsigned k = 0; k < sizeof off; k++) sys(0, off[k], 0, 0);   /* OS_WriteC */
    roger_cli("ScreenLoad <Repton4$Dir>.LoadScreen");
}

static const struct roger_config config = {
    .title = "Ego: Repton 4 (recompiled)",
    .image_path = "../game/!Repton4/ObjCode,00008000-0001e9b8",
    .image = &recomp_image,
    .entry = 0x1E9B8,
    .floppy = "../extracted/disc1.img",
    .appdir = "../game",
    .dir_prefixes = prefixes,
    .variables = variables,
    .initial_mode = 13,
    .wimpslot = 640 * 1024,                      /* !Run */
    /* the keys (key transition events): Z left, X right, ' up, / down,
     * Return drop (as Define Keys shows them; it can change them). A
     * controller holds those. The game reads Acorn's joystick too
     * (Joystick_Read), but only once it's chosen in the options, so the
     * controller isn't offered as that joystick: it would do nothing
     * until then. */
    .pad_map = "left=Z,right=X,up=',down=/,a=Return,b=Return,start=Return",
    .init = init,
};

int main(int argc, char **argv) { roger_main(argc, argv, &config); }
