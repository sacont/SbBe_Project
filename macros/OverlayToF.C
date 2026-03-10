void OverlayToF(Bool_t normalize = kTRUE,
                const char* hname = "ToF",
                const char* outpng = "ToF_overlay.png")
{
  // ---- helper: try open filename, else try alternate ----
  TFile* f0 = TFile::Open("0degree0.root","READ");
  if (!f0 || f0->IsZombie()) { if (f0) delete f0; f0 = TFile::Open("0degree.root","READ"); }

  TFile* f1 = TFile::Open("1degree0.root","READ");
  if (!f1 || f1->IsZombie()) { if (f1) delete f1; f1 = TFile::Open("1degree.root","READ"); }

  TFile* f12 = TFile::Open("12degree0.root","READ");
  if (!f12 || f12->IsZombie()) { if (f12) delete f12; f12 = TFile::Open("12degree.root","READ"); }

  TFile* f30 = TFile::Open("30degree0.root","READ");
  if (!f30 || f30->IsZombie()) { if (f30) delete f30; f30 = TFile::Open("30degree.root","READ"); }

  if (!f0 || f0->IsZombie())  { printf("Could not open 0degree file\n");  return; }
  if (!f1 || f1->IsZombie())  { printf("Could not open 1degree file\n");  return; }
  if (!f12 || f12->IsZombie()){ printf("Could not open 12degree file\n"); return; }
  if (!f30 || f30->IsZombie()){ printf("Could not open 30degree file\n"); return; }

  // ---- get histograms ----
  TH1* h0  = (TH1*)f0->Get(hname);
  TH1* h1h = (TH1*)f1->Get(hname);
  TH1* h12h= (TH1*)f12->Get(hname);
  TH1* h30h= (TH1*)f30->Get(hname);

  if (!h0)   { printf("Histogram '%s' not found in 0degree file\n",  hname);  return; }
  if (!h1h)  { printf("Histogram '%s' not found in 1degree file\n",  hname);  return; }
  if (!h12h) { printf("Histogram '%s' not found in 12degree file\n", hname);  return; }
  if (!h30h) { printf("Histogram '%s' not found in 30degree file\n", hname);  return; }

  // ---- clone so histograms survive after closing files ----
  TH1* H0  = (TH1*)h0->Clone("H0_overlay");   H0->SetDirectory(0);
  TH1* H1  = (TH1*)h1h->Clone("H1_overlay");  H1->SetDirectory(0);
  TH1* H12 = (TH1*)h12h->Clone("H12_overlay");H12->SetDirectory(0);
  TH1* H30 = (TH1*)h30h->Clone("H30_overlay");H30->SetDirectory(0);

  // ---- optional normalization (compare shapes) ----
  if (normalize) {
    if (H0->Integral()  > 0) H0->Scale(1.0 / H0->Integral());
    if (H1->Integral()  > 0) H1->Scale(1.0 / H1->Integral());
    if (H12->Integral() > 0) H12->Scale(1.0 / H12->Integral());
    if (H30->Integral() > 0) H30->Scale(1.0 / H30->Integral());
  }

  // ---- style ----
  gStyle->SetOptStat(0);

  H0->SetLineColor(kBlack);   H0->SetLineWidth(2);
  H1->SetLineColor(kRed+1);   H1->SetLineWidth(2);
  H12->SetLineColor(kBlue+1); H12->SetLineWidth(2);
  H30->SetLineColor(kGreen+2);H30->SetLineWidth(2);

  // y-axis range: pick max of the four
  Double_t m0 = H0->GetMaximum();
  Double_t m1 = H1->GetMaximum();
  Double_t m12= H12->GetMaximum();
  Double_t m30= H30->GetMaximum();
  Double_t ymax = m0;
  if (m1  > ymax) ymax = m1;
  if (m12 > ymax) ymax = m12;
  if (m30 > ymax) ymax = m30;
  H0->SetMaximum(1.15 * ymax);

  H0->SetTitle("TOF Overlay;ToF (ns);Counts (normalized)" );
  if (!normalize) H0->SetTitle("TOF Overlay;ToF (ns);Counts");

  // ---- draw ----
  TCanvas* c = new TCanvas("cToFOverlay","ToF Overlay",1000,700);
  c->SetGrid();

  H0->Draw("HIST");
  H1->Draw("HIST SAME");
  H12->Draw("HIST SAME");
  H30->Draw("HIST SAME");

  TLegend* leg = new TLegend(0.62,0.68,0.88,0.88);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(H0,  "0 degree",  "l");
  leg->AddEntry(H1,  "1 degree",  "l");
  leg->AddEntry(H12, "12 degree", "l");
  leg->AddEntry(H30, "30 degree", "l");
  leg->Draw();

  c->SaveAs(outpng);

  // ---- cleanup files ----
  f0->Close();  delete f0;
  f1->Close();  delete f1;
  f12->Close(); delete f12;
  f30->Close(); delete f30;

  printf("Saved: %s\n", outpng);
}