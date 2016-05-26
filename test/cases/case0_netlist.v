/////////////////////////////////////////////////////////////
// Created by: Synopsys DC Expert(TM) in wire load mode
// Version   : K-2015.06-SP3
// Date      : Thu Mar 24 08:39:19 2016
/////////////////////////////////////////////////////////////


module mul2 ( M3, M2, M1, M0, A1, A0, B1, B0 );     
    output M3, M2, M1, M0;
    input A1, A0;
    input B1, B0;
    wire n1, n2, n3, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14;

    NOT1 U1 ( .A(n13), .Y(n1) );
    NOT1 U2 ( .A(n11), .Y(n2) );
    NOT1 U3 ( .A(n9), .Y(n3) );
    NOT1 U4 ( .A(n14), .Y(M0) );
    NOT1 U5 ( .A(B1), .Y(n5) );
    NOR2 U6 ( .A(n6), .B(n7), .Y(M3) );
    NAND2 U7 ( .A(B1), .B(B0), .Y(n7) );
    NAND2 U8 ( .A(A1), .B(A0), .Y(n6) );
    NOR2 U9 ( .A(n5), .B(n8), .Y(M2) );
    NAND2 U10 ( .A(A1), .B(n3), .Y(n8) );
    NOR2 U11 ( .A(n10), .B(n11), .Y(n9) );
    NAND2 U12 ( .A(n12), .B(n1), .Y(M1) );
    NOR2 U13 ( .A(n10), .B(n2), .Y(n13) );
    NAND2 U14 ( .A(n2), .B(n10), .Y(n12) );
    NAND2 U15 ( .A(B1), .B(A0), .Y(n10) );
    NAND2 U16 ( .A(B0), .B(A1), .Y(n11) );
    NAND2 U17 ( .A(A0), .B(B0), .Y(n14) );
endmodule

