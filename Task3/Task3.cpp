#include <uf.h>
#include <uf_curve.h>
#include <uf_modl.h>
#include <uf_sket.h>

void ufusr(char* param, int* retcode, int paramLen) {
    UF_initialize();

    tag_t sketch1 = NULL_TAG;
    char scn1[129] = "Sketch_Z119";
    double matrix1[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 119.0};
    int option = 2;
    tag_t obj[2] = {NULL_TAG, NULL_TAG};
    int ref[2] = {0, 0};
    int plane_dir = 1;

    UF_SKET_initialize_sketch(scn1, &sketch1);
    UF_SKET_create_sketch(scn1, option, matrix1, obj, ref, plane_dir, &sketch1);

    double z1 = 119.0;
    tag_t outer_c1, inner_c1;
    double p_out1[3] = {70.0, 0.0, z1};
    double p_out2[3] = {-70.0, 0.0, z1};
    double p_out3[3] = {0.0, 70.0, z1};
    UF_CURVE_create_arc_thru_3pts(2, p_out1, p_out2, p_out3, &outer_c1);

    double p_in1[3] = {25.0, 0.0, z1};
    double p_in2[3] = {-25.0, 0.0, z1};
    double p_in3[3] = {0.0, 25.0, z1};
    UF_CURVE_create_arc_thru_3pts(2, p_in1, p_in2, p_in3, &inner_c1);

    tag_t sk_objs1[2] = {outer_c1, inner_c1};
    UF_SKET_add_objects(sketch1, 2, sk_objs1);

    uf_list_p_t ext_list1;
    UF_MODL_create_list(&ext_list1);
    UF_MODL_put_list_item(ext_list1, sketch1);

    char lim1s[15] = "0.0", lim1e[15] = "4.0";
    char* lim1[2] = {lim1s, lim1e};
    char taper1[5] = "0.0";
    double dir1[3] = {0.0, 0.0, 1.0};
    double pt1[3] = {0.0, 0.0, 0.0};

    uf_list_p_t feat_list1;
    UF_MODL_create_extruded(ext_list1, taper1, lim1, pt1, dir1, UF_NULLSIGN,
                            &feat_list1);
    UF_MODL_delete_list(&feat_list1);
    UF_MODL_delete_list(&ext_list1);

    tag_t sketch2 = NULL_TAG;
    char scn2[129] = "Sketch_Z123";
    double matrix2[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 123.0};

    UF_SKET_initialize_sketch(scn2, &sketch2);
    UF_SKET_create_sketch(scn2, option, matrix2, obj, ref, plane_dir, &sketch2);

    double z2 = 123.0;
    tag_t circ2;
    double pc1[3] = {70.0, 0.0, z2};
    double pc2[3] = {-70.0, 0.0, z2};
    double pc3[3] = {0.0, 70.0, z2};
    UF_CURVE_create_arc_thru_3pts(2, pc1, pc2, pc3, &circ2);

    tag_t sk_objs2[1] = {circ2};
    UF_SKET_add_objects(sketch2, 1, sk_objs2);

    uf_list_p_t ext_list2;
    UF_MODL_create_list(&ext_list2);
    UF_MODL_put_list_item(ext_list2, sketch2);

    char lim2s[15] = "0.0", lim2e[15] = "16.0";
    char* lim2[2] = {lim2s, lim2e};
    char taper2[5] = "0.0";
    double dir2[3] = {0.0, 0.0, 1.0};
    double pt2[3] = {0.0, 0.0, 0.0};

    uf_list_p_t feat_list2;
    UF_MODL_create_extruded(ext_list2, taper2, lim2, pt2, dir2, UF_NULLSIGN,
                            &feat_list2);
    UF_MODL_delete_list(&feat_list2);
    UF_MODL_delete_list(&ext_list2);

    tag_t sketch3 = NULL_TAG;
    char scn3[129] = "Sketch_Z139";
    double matrix3[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 123.0 + 16.0};

    UF_SKET_initialize_sketch(scn3, &sketch3);
    UF_SKET_create_sketch(scn3, option, matrix3, obj, ref, plane_dir, &sketch3);

    double z3 = 139.0;
    tag_t circ3;
    double r = 55.0 / 2;
    double p_c3_1[3] = {r, 0.0, z3};
    double p_c3_2[3] = {-r, 0.0, z3};
    double p_c3_3[3] = {0.0, r, z3};
    UF_CURVE_create_arc_thru_3pts(2, p_c3_1, p_c3_2, p_c3_3, &circ3);

    tag_t sk_objs3[1] = {circ3};
    UF_SKET_add_objects(sketch3, 1, sk_objs3);

    uf_list_p_t ext_list3;
    UF_MODL_create_list(&ext_list3);
    UF_MODL_put_list_item(ext_list3, sketch3);

    char lim3s[15] = "0.0", lim3e[15] = "5.0";
    char* lim3[2] = {lim3s, lim3e};
    char taper3[5] = "0.0";
    double dir3[3] = {0.0, 0.0, -1.0};
    double pt3[3] = {0.0, 0.0, 0.0};

    uf_list_p_t feat_list3;
    UF_MODL_create_extruded(ext_list3, taper3, lim3, pt3, dir3, UF_NEGATIVE,
                            &feat_list3);
    UF_MODL_delete_list(&feat_list3);
    UF_MODL_delete_list(&ext_list3);

    tag_t sketch4 = NULL_TAG;
    char scn4[129] = "Sketch_Z139_Holes";
    double matrix4[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 139.0};

    UF_SKET_initialize_sketch(scn4, &sketch4);
    UF_SKET_create_sketch(scn4, option, matrix4, obj, ref, plane_dir, &sketch4);

    double z4 = 139.0;
    tag_t circles[4];
    double r_hole = 13.0 / 2;
    double r_pos = 100.0 / 2;

    double p_0_1[3] = {r_pos + r_hole, 0.0, z4};
    double p_0_2[3] = {r_pos - r_hole, 0.0, z4};
    double p_0_3[3] = {r_pos, r_hole, z4};
    UF_CURVE_create_arc_thru_3pts(2, p_0_1, p_0_2, p_0_3, &circles[0]);

    double p_90_1[3] = {0.0, r_pos + r_hole, z4};
    double p_90_2[3] = {0.0, r_pos - r_hole, z4};
    double p_90_3[3] = {-r_hole, r_pos, z4};
    UF_CURVE_create_arc_thru_3pts(2, p_90_1, p_90_2, p_90_3, &circles[1]);

    double p_180_1[3] = {-r_pos + r_hole, 0.0, z4};
    double p_180_2[3] = {-r_pos - r_hole, 0.0, z4};
    double p_180_3[3] = {-r_pos, r_hole, z4};
    UF_CURVE_create_arc_thru_3pts(2, p_180_1, p_180_2, p_180_3, &circles[2]);

    double p_270_1[3] = {0.0, -r_pos + r_hole, z4};
    double p_270_2[3] = {0.0, -r_pos - r_hole, z4};
    double p_270_3[3] = {r_hole, -r_pos, z4};
    UF_CURVE_create_arc_thru_3pts(2, p_270_1, p_270_2, p_270_3, &circles[3]);

    UF_SKET_add_objects(sketch4, 4, circles);

    uf_list_p_t ext_list4;
    UF_MODL_create_list(&ext_list4);
    UF_MODL_put_list_item(ext_list4, sketch4);

    char lim4s[15] = "0.0";
    char lim4e[15] = "16.0";
    char* lim4[2] = {lim4s, lim4e};
    char taper4[5] = "0.0";
    double dir4[3] = {0.0, 0.0, -1.0};
    double pt4[3] = {0.0, 0.0, 0.0};

    uf_list_p_t feat_list4;
    UF_FEATURE_SIGN sign4 = UF_NEGATIVE;

    UF_MODL_create_extruded(ext_list4, taper4, lim4, pt4, dir4, sign4,
                            &feat_list4);

    UF_MODL_delete_list(&feat_list4);
    UF_MODL_delete_list(&ext_list4);

    UF_terminate();
}

int ufusr_ask_unload(void) { return UF_UNLOAD_IMMEDIATELY; }

