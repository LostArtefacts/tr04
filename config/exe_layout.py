"""Tomb4's standalone link, through the shared original-address layout."""

import orig_layout


def link_exe(objects: list[str], exe_objects: list[str], feats: list[str]) -> None:
    orig_layout.link(exe_objects, runtime=["libcmt.lib"], data_libraries=["dxguid.lib"])
