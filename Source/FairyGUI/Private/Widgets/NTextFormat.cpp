#include "Widgets/NTextFormat.h"

#include "FairyCommons.h"
#include "Engine/FontFace.h"
#include "UI/UIConfig.h"
#include "UI/UIPackage.h"

FNTextFormat::FNTextFormat() :
    Size(12),
    Color(FColor::White),
    bBold(false),
    bItalic(false),
    bUnderline(false),
    LineSpacing(3),
    LetterSpacing(0),
    Align(EAlignType::Left),
    VerticalAlign(EVerticalAlignType::Top),
    OutlineColor(FColor::Black),
    OutlineSize(0),
    ShadowColor(FColor::Black),
    ShadowOffset(0, 0)
{

}

bool FNTextFormat::EqualStyle(const FNTextFormat& AnotherFormat) const
{
    return Size == AnotherFormat.Size && Color == AnotherFormat.Color
        && bBold == AnotherFormat.bBold && bUnderline == AnotherFormat.bUnderline
        && bItalic == AnotherFormat.bItalic
        && Align == AnotherFormat.Align;
}


UFontFace* FindFontFace(const FString& Name)
{
    TArray PotentialPaths = {
        FString::Printf(TEXT("/Game/Fonts/%s"), *Name),
        FString::Printf(TEXT("/Game/Fonts/%s_Font"), *Name),
        FString::Printf(TEXT("/Game/UI/Fonts/Fonts/%s"), *Name),
        FString::Printf(TEXT("/Game/UI/Fonts/Fonts/%s_Font"), *Name),
    };

    UFontFace* FontFace = nullptr;
    for (FString PotentialPath : PotentialPaths)
    {
        FontFace = LoadObject<UFontFace>(nullptr, *PotentialPath);
        if (FontFace != nullptr)
        {
            break;
        }
    }

    return FontFace;
}

FTextBlockStyle FNTextFormat::GetStyle() const
{
    FTextBlockStyle Style;

    const FString& FontFace = Face.IsEmpty() ? FUIConfig::Config.DefaultFont : Face;
    if (!FontFace.StartsWith("ui://"))
    {
        auto Font = UUIPackageStatic::Get().Fonts.FindRef(FontFace);
        if (Font == nullptr)
        {
            Font = FindFontFace(*FontFace);
        }
        if (Font != nullptr)
        {
            FSlateFontInfo SlateFont(Font.Get(), Size * 0.75f);
            SlateFont.OutlineSettings.OutlineSize = OutlineSize;
            SlateFont.OutlineSettings.OutlineColor = OutlineColor;
            Style.SetFont(SlateFont);
        }
        else
        {
            UE_LOG(LogFairyGUI, Log, TEXT("Font '%s' not found, using default font."), *FontFace);
            FSlateFontInfo SlateFont = FCoreStyle::GetDefaultFontStyle(*FontFace, Size * 0.75f);
            SlateFont.OutlineSettings.OutlineSize = OutlineSize;
            SlateFont.OutlineSettings.OutlineColor = OutlineColor;
            Style.SetFont(SlateFont);
        }
    }

    Style.SetColorAndOpacity(FSlateColor(FLinearColor(Color)));
    Style.SetShadowOffset(ShadowOffset);
    Style.SetShadowColorAndOpacity(ShadowColor);

    return MoveTemp(Style);
}
