import pandas as pd

file_path = "d:/Documents/Utem's Works/#Year2Sem1/BITP2113 ALGORITHM ANALYSIS/Project AA/spotify_songs_dataset_cleaned_language.csv"

df_cleaned = df.dropna(subset=['language'])

df_cleaned['song_id'] = ['SP' + str(i).zfill(4) for i in range(1, len(df_cleaned) + 1)]


output_path = "d:/Documents/Utem's Works/#Year2Sem1/BITP2113 ALGORITHM ANALYSIS/Project AA/spotify_songs_dataset_update.csv"
df_cleaned.to_csv(output_path, index=False)

print(f"Updated dataset saved to: {output_path}")