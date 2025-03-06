# A bit of ~~philosophy~~ common sense

One can only write better code, by learning from the past. Now, DRM did *not* learn from the past one bit. As a result of such, the "desktop of the future" now uses a non-standardized mess of properties attached to connectors, CRTCs and planes. Digging into the linux source code, there does seem to be at least an attempt at standardizing these properties, but seeing how half of the properties mentioned in those code comments only being implemented by one or two drivers, it's safe to say that this is a lost cause.

# Idea

This document relies on the amazing [drmdb](https://drmdb.emersion.fr) from [emersion](https://emersion.fr/), which is a database of user submitted drm "dumps". Anyone can submit a dump, so there's no guarantee that the information here is accurate, however, it's the best we have.

The first thing you'll notice when looking at this database, is that there's a bunch of properties that only a few drivers implement. We're not going to support those, we would be spending way too long on that (besides, what the heck is "rockchip" anyways.. and why the hell does nouveau implement a "vibrant_hue" property???). The idea is, we're going to look at a subset of drivers and decide from there which properties we'll support.

The drivers we're going to look at are:
- amdgpu
- i915
- nvidia_drm
- bochs-drm (qemu std)
- virtio_gpu (qemu virtio)
- qxl (qemu qxl)
- vmwgfx (vmware)
- simpledrm (driver-less hardware)

# Properties

Starting at all the virtual drivers, as those mostly have the same properties. Let's make a table of all supported properties, and then we can decide which ones we want to support.

| CRTC Property | bochs-drm | virtio_gpu | qxl | vmwgfx | simpledrm |
|---------------|-----------|------------|-----|--------|-----------|
| ACTIVE        | X         | X          | X   | X      | X         |
| MODE_ID       | X         | X          | X   | X      | X         |
| OUT_FENCE_PTR | X         | X          | X   | X      | X         |
| VRR_ENABLED   | X         | X          | X   | X      | X         |

| Connector Property | bochs-drm | virtio_gpu | qxl | vmwgfx | simpledrm |
|--------------------|-----------|------------|-----|--------|-----------|
| CRTC_ID            | X         | X          | X   | X      | X         |
| DPMS               | X         | X          | X   | X      | X         |
| EDID               | X         | X          |     |        | X         |
| TILE               | X         | X          | X   | X      | X         |
| hotplug_mode_update|           |            | X   | X      |           |
| link-status        | X         | X          | X   | X      | X         |
| non-desktop        | X         | X          | X   | X      | X         |
| suggested_X/Y      |           |            | X   | X      |           |

| Plane Property | bochs-drm | virtio_gpu | qxl | vmwgfx | simpledrm |
|----------------|-----------|------------|-----|--------|-----------|
| CRTC_X/Y/W/H   | X         | X          | X   | X      | X         |
| CRTC_ID        | X         | X          | X   | X      | X         |
| FB_DAMAGE_CLIPS|           |            |     | X      | X         |
| FB_ID          | X         | X          | X   | X      | X         |
| IN_FENCE_FD    | X         | X          | X   | X      | X         |
| IN_FORMATS     |           |            |     |        | X         |
| SRC_X/Y/W/H    | X         | X          | X   | X      | X         |
| type           | X         | X          | X   | X      | X         |

Now this is a relatively small list of properties. Most of these here, having worked with DRM before, are essential for it to work. Let's only look at those that aren't that obvious:
1. `EDID`: According to kernel docs, this is a legacy property and replaced with "ACTIVE". Since all of the drivers support that property, we're not going to support this one.
2. `hotplug_mode_update`, `suggested_X/Y`, `FB_DAMAGE_CLIPS`: Not well supported, so we don't support these.
3. `IN_FORMATS`: Odd. This property is pretty essential for planes, but not supported by most virtual drivers. Will still support.

Let's take the remaining properties and mark them as supported.

|   Connector   |     CRTC     |     Plane     |
|---------------|--------------|---------------|
| ACTIVE        | CRTC_ID      | CRTC_X/Y/W/H  |
| MODE_ID       | non-desktop  | CRTC_ID       |
| OUT_FENCE_PTR | TILE         | FB_ID         |
| VRR_ENABLED   | link-status  | IN_FENCE_FD   |
|               |              | IN_FORMATS    |
|               |              | SRC_X/Y/W/H   |
|               |              | type          |

Before we look at the other drivers, I have a few more things to say about these properties. While most of them are essential for drm to work, there are some that are not and we shall treat them accordingly:
1. `OUT_FENCE_PTR`, `IN_FENCE_FD`: These are relatively new properties, and are used for explicit sync. It's important to support both implicit sync and explicit sync.
2. `VRR_ENABLED`: I'm surprised all virtual drivers even support this, especially since none of them support the vrr_capable property. Either way, there's no need to handle anything special for this property, as it's just a boolean.
3. `TILE`/`non-desktop`: Even if supported by all drivers, these shouldn't be relied upon.
4. `IN_FORMATS`: No clue why this isn't very well supported, but let's declare right now, if it isn't present, we only support XRGB8888 with linear modifier (or none? TODO).

In the last section all properties will be listed, so no need to memorize this.

Let's take a look at the big drivers now. First, let's look at new properties on the connector object:

| Connector Property     | nvidia-drm | amdgpu | i915 |
|------------------------|------------|--------|------|
| Colorspace             | X          | X      | X    |
| Content Protection     |            | X      | X    |
| HDCP Content Type      |            | X      | X    |
| HDR_OUTPUT_METADATA    | X          | X      | X    |
| PATH                   |            | X      | X    |
| WRITEBACK_FB_ID        |            | X      |      |
| WRITEBACK_OUT_FENCE_PTR|            | X      |      |
| WRITEBACK_PIXEL_FORMATS|            | X      |      |
| abm_level              |            | X      |      |
| aspect_ratio           |            |        | X    |
| audio                  |            | X      | X    |
| coherent               |            | X      |      |
| content_type           |            | X      | X    |
| dither                 |            | X      |      |
| freesync               |            | X      |      |
| freesync_capable       |            | X      |      |
| load_detection         |            | X      |      |
| max_bpc                |            | X      | X    |
| panel_orientation      |            | X      | X    |
| privacy-screen hw-state|            | X      |      |
| privacy-screen sw-state|            | X      |      |
| scaling_mode           |            | X      | X    |
| subconnector           |            | X      | X    |
| underscan              |            | X      |      |
| underscan_v/hborder    |            | X      |      |

These results are already wild! I have no idea what AMD was cooking here, half of these properties make absolutely no sense. I mean, HDCP? Really now? DRM does not have an API for HDCP and likely will never in the future, so why is this here? Same with properties like "audio" or "abm_level". These are just.. wild. And they probably don't even work in the first place. Let's take a look at the properties that do make sense:
- `Colorspace`,`HDR_OUTPUT_METADATA`: These are properties that are essential for HDR. We should support these.
.. and that's it. These are the only properties that are supported by all drivers and there's no other property that is so essential that we should support it. Let's take a look at the CRTC properties now (skipping all NV_, AMD_ or VALVE1_ properties):

|  CRTC Property  | nvidia-drm | amdgpu | i915 |
| ----------------|------------|--------|------|
| CTM             | X          | X      | X    |
| DEGAMMA_LUT     | X          | X      | X    |
| DEGAMMA_LUT_SIZE| X          | X      | X    |
| GAMMA_LUT       | X          | X      | X    |
| GAMMA_LUT_SIZE  | X          | X      | X    |
| SCALING_FILTER  |            |        | X    |

It's pretty obvious which ones we'll support:
- `DEGAMMA_LUT`, `DEGAMMA_LUT_SIZE`: Useful for converting gamma to linear space, so we'll support these.
- `CTM`: Color transformation matrix, essential for color correction.
- `GAMMA_LUT`, `GAMMA_LUT_SIZE`: Useful for converting linear to gamma space, so we'll support these.

As far as I have understood, your average color space is "gamma", meaning twice as high value != twice as bright. A degamma look up table turns this into linear space, then you can apply a color transformation matrix on it with sane results, and then apply a gamma look up table to get the final result.

Finally, let's take a look at the plane properties:
|  Plane Property  | nvidia-drm | amdgpu | i915 |
|------------------|------------|--------|------|
| COLOR_ENCODING   |            | X      | X    |
| COLOR_RANGE      |            | X      | X    |
| SCALING_FILTER   |            |        | X    |
| SIZE_HINTS       |            |        | X    |
| alpha            | X          | X      | X    |
| pixel_blend_mode | X          | X      | X    |
| rotation         | X          | X      | X    |
| zpos             |            | X      | X    |

I'm just going to go ahead and say that only one of these properties should be supported. Some of them are just stupid again, but even the remaining ones like "alpha" and "pixel_blend_mode" just don't seem useful at all. This kind of compositing should be done in the compositor, not in the driver:
- `rotation`: Takes work off the compositor to support rotated displays, so we'll support this.

# Supported Properties

Here's the final list of properties we'll support:
|      Connector      |       CRTC       |     Plane     |
|---------------------|------------------|---------------|
| ACTIVE              | CRTC_ID          | CRTC_X/Y/W/H  |
| MODE_ID             | CTM              | CRTC_ID       |
| OUT_FENCE_PTR       | TILE             | FB_ID         |
| VRR_ENABLED         | link-status      | IN_FENCE_FD   |
| Colorspace          | non-desktop      | IN_FORMATS    |
| HDR_OUTPUT_METADATA | DEGAMMA_LUT      | SRC_X/Y/W/H   |
|                     | DEGAMMA_LUT_SIZE | type          |
|                     | GAMMA_LUT        | rotation      |
|                     | GAMMA_LUT_SIZE   |               |

Here's some notes on these properties:
- `OUT_FENCE_PTR`, `IN_FENCE_FD`: These are used for explicit sync, but since these are relatively new, we'll also support implicit sync.
- `VRR_ENABLED`, `vrr_capable`: The user should NOT rely on vrr_capable existing when enabling VRR.
- `link-status`: Handling this property is a bit odd. Let's say an atomic modeset fails if any of the contained connectors have a link-status of "bad". Making sure the end user knows how to handle this accordingly.
- `TILE`: I don't know how this property works. I can't really implement it.
